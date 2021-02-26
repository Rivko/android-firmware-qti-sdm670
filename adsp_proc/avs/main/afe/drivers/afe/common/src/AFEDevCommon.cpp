/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDevCommon.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEDevCommon.cpp

DESCRIPTION: AFE Service Common Driver Functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDevCommon.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/22/17    SJ      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDevCommon.h"
#include "DDIGPIOInt.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDITlmm.h"

/*=====================================================================
 Globals
 ======================================================================*/


/* This structure will get updated during boot up*/
/* Once updated, this structure will be used for reading only values */

typedef struct afe_dev_lpass_hw_info
{
   bool_t                  is_initialized;
   HwdLpassPropertyType    hw_prop;
}afe_dev_lpass_hw_info_t;


/* Global structure to hold LPASS h/w info */
afe_dev_lpass_hw_info_t    g_lpass_hw_info;


/*=====================================================================
 Functions
 ======================================================================*/
/**
  @brief Init the LPASS h/w info device config

  @param[in] None
  @param[out] None

  @return  ADSPResult

 */

ADSPResult afe_dev_init_lpass_hw_info()
{
   DALSYSPropertyVar prop_var;
   const char *devId = "LPASS";

   /* Clear the struct */
   memset(&g_lpass_hw_info, 0, sizeof(g_lpass_hw_info));

   /* Query device cfg init*/
   DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(devId, pHandle))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPASS devCfg: Error getting prop handle");
      return ADSP_EFAILED;
   }

   /* Read property ptr */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue(pHandle, "LpassPropStructPtr", 0, &prop_var))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPASS DevCfg: Error getting property struct ptr");
      return ADSP_EFAILED;
   }

   HwdLpassPropertyType *lpass_prop_dev_cfg_ptr = (HwdLpassPropertyType *)prop_var.Val.pStruct;

   /* Fetch the LPASS h/w info device config */
   g_lpass_hw_info.hw_prop.hw_ver_reg_base = lpass_prop_dev_cfg_ptr->hw_ver_reg_base;
   g_lpass_hw_info.hw_prop.codec_intr_gpio_type = lpass_prop_dev_cfg_ptr->codec_intr_gpio_type;
   g_lpass_hw_info.hw_prop.codec_intr_type = lpass_prop_dev_cfg_ptr->codec_intr_type;
   g_lpass_hw_info.hw_prop.gpio_num_cdc_int = lpass_prop_dev_cfg_ptr->gpio_num_cdc_int;

   /* LPASS h/w info is initialized */
   g_lpass_hw_info.is_initialized = TRUE;

   return ADSP_EOK;
}

lpasshwio_irq_type_t afe_get_cdc_intr_type()
{
   return ((TRUE == g_lpass_hw_info.is_initialized) ? (g_lpass_hw_info.hw_prop.codec_intr_type) : IRQ_TYPE_UNSUPPORTED);
}

lpasshwio_gpio_type_t afe_get_cdc_intr_gpio_type()
{
   return ((TRUE == g_lpass_hw_info.is_initialized) ? (g_lpass_hw_info.hw_prop.codec_intr_gpio_type) : GPIO_TYPE_UNSUPPORTED);
}

uint32_t afe_get_cdc_intr_gpio_num()
{
   return ((TRUE == g_lpass_hw_info.is_initialized) ? (g_lpass_hw_info.hw_prop.gpio_num_cdc_int) : AFE_GPIO_NUM_INVALID);
}


/**
 * GPIO Initialization Routine
 * Steps:
 * 1)Query device config for the interrupt properties
 * 2)Attach to the TLMM
 * 3)Attach to GPIOInt
 * 4)Create a pin configuration for the required GPIO Pin
 * 5)Call the DAL API to configure the GPIO pin using above configuration
 * 6)Set the appropriate trigger for the interrupt (active high/active low/rising edge/trailing edge)
 * 7)Clear the interrupt
 * 8)Enable the interrupt
 */

ADSPResult afe_device_gpio_init(const char *prop_string_ptr,
                                gpio_int_properties_t *gpio_int_prop_ptr,
                                uint32_t ext_gpio_num)
{
   DALResult            result = DAL_SUCCESS;
   DALGpioSignalType    pin_config;
   uint8_t              l2vic_int_vector, n_gpio;
   DalDeviceHandle      *tlmm_hptr = NULL,*gpio_int_hptr = NULL;

   //structure which holds properties for DAL
   DALSYSPropertyVar system_property_var;

   //declare property handle variable name
   DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property);

   if (NULL == prop_string_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Property string NULL");
      return ADSP_EFAILED;
   }

   /* Set the config flag to FALSE */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   //Attach to TLMM
   if ((DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_hptr) != DAL_SUCCESS) || (tlmm_hptr == NULL))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Failed to attach to TLMM");
      return ADSP_EFAILED;
   }

   /* Save the DAL TLMM handle */
   gpio_int_prop_ptr->tlmm_hptr = tlmm_hptr;

   //Acquire a handle to GPIOInt
   if ((DAL_DeviceAttach(DALDEVICEID_GPIOINT, &gpio_int_hptr) != DAL_SUCCESS) || (gpio_int_hptr == NULL))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Failed to acquire a handle to GPIOInt");
      return ADSP_EFAILED;
   }

   /* Save the DAL GPIOInt handle */
   gpio_int_prop_ptr->gpio_int_hptr = gpio_int_hptr;

   //Get the property handle
   DALSYS_GetDALPropertyHandle(DALDEVICEID_GPIOINT, interrupt_property);

   //Query the interrupt properties. Use the string maintained in core BSP configuration xml
   result = DALSYS_GetPropertyValue(interrupt_property, prop_string_ptr, 0, &system_property_var);

   if ((DAL_SUCCESS == result) && (system_property_var.Val.pbVal[1] != 0xFF))
   {
      gpio_int_prop_ptr->intr_prop.l2vic_int_vector = l2vic_int_vector  = system_property_var.Val.pbVal[0];
      gpio_int_prop_ptr->n_gpio = n_gpio  = system_property_var.Val.pbVal[1];
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Failed to query GPIO properties");
      return ADSP_EFAILED;
   }

   //If client passes valid GPIO number then store that
   if (AFE_GPIO_NUM_INVALID != ext_gpio_num)
   {
      gpio_int_prop_ptr->n_gpio = n_gpio  = (uint8_t)ext_gpio_num;
   }

   //create a GPIO configuration for the required pin
   //function value 0 indicates that this is pin is a dedicated interrupt pin (not a general purpose pin)
   pin_config = DAL_GPIO_CFG(n_gpio, 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);

   //call the TLMM DAL API to configure the GPIO
   if (DalTlmm_ConfigGpio(tlmm_hptr, pin_config, DAL_TLMM_GPIO_ENABLE) != DAL_SUCCESS)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Failed to configure GPIO Pin");
      return ADSP_EFAILED;
   }

   //set the mapping between the GPIO pin and L2VIC interrupt
   GPIOInt_SetDirectConnectGPIOMapping(gpio_int_hptr, n_gpio, l2vic_int_vector);

   //set the trigger for the interrupt as rising edge (This is recommended for codec interrupt)
   if (DAL_SUCCESS != (result = GPIOInt_SetTrigger(gpio_int_hptr,
                                                   n_gpio,
                                                   (GPIOIntTriggerType)gpio_int_prop_ptr->intr_prop.trigger_type)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT: Failed to set trigger for GPIOInt");
      return ADSP_EFAILED;
   }

   //clear the interrupt, this is for safety to clear any spurious interrupts, if any, on that line
   if (GPIOInt_ClearInterrupt(gpio_int_hptr, n_gpio) != DAL_SUCCESS)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT:Failed to clear interrupt");
      return ADSP_EFAILED;
   }

   //Enable GPIO interrupt
   if (GPIOInt_EnableInterrupt(gpio_int_hptr, n_gpio) != DAL_SUCCESS)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INIT:Failed to enable GPIO interrupt");
      return ADSP_EFAILED;
   }

   /* Save the l2vic and GPIO numbers */
   gpio_int_prop_ptr->intr_prop.l2vic_int_vector = l2vic_int_vector;
   gpio_int_prop_ptr->n_gpio = n_gpio;

   /* Set the GPIO config flag to TRUE */
   gpio_int_prop_ptr->is_gpio_configured = TRUE;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "GPIO INIT: Configured GPIO[%lu] successfully", n_gpio);

   return ADSP_EOK;
}

ADSPResult afe_device_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr)
{
   ADSPResult result = ADSP_EOK;

   /* Disable GPIO interrupt */
   if ((NULL != gpio_int_prop_ptr->gpio_int_hptr) &&
       (TRUE == gpio_int_prop_ptr->intr_prop.is_interrupt_enabled))
   {
      if (GPIOInt_DisableInterrupt(gpio_int_prop_ptr->gpio_int_hptr, gpio_int_prop_ptr->n_gpio) != DAL_SUCCESS)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO DEINIT:Failed to disable interrupt");
         result = ADSP_EFAILED;
      }
      else
      {
         gpio_int_prop_ptr->intr_prop.is_interrupt_enabled = FALSE;
      }
   }

   /* Detach GPIOInt */
   if (NULL != gpio_int_prop_ptr->gpio_int_hptr)
   {
      if (DAL_DeviceDetach(gpio_int_prop_ptr->gpio_int_hptr) != DAL_SUCCESS)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO DEINIT:Failed to detach from GPIOInt");
         result = ADSP_EFAILED;
      }
      else
      {
         gpio_int_prop_ptr->gpio_int_hptr = NULL;
      }
   }

   /* Detach TLMM */
   if (NULL != gpio_int_prop_ptr->tlmm_hptr)
   {
      if (DAL_DeviceDetach(gpio_int_prop_ptr->tlmm_hptr) != DAL_SUCCESS)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO DEINIT:Failed to detach from TLMM");
         result = ADSP_EFAILED;
      }
      else
      {
         gpio_int_prop_ptr->tlmm_hptr = NULL;
      }
   }

   /* Clear the GPIO config flag to FALSE */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "GPIO DEINIT:GPIO[%lu] interrupt de-init successful", gpio_int_prop_ptr->n_gpio);

   return result;
}


ADSPResult afe_device_gated_intr_register(afe_dev_intr_prop_t *p_int_prop)
{
   DALResult         result = DAL_SUCCESS;
   DalDeviceHandle   *int_ctrl_hptr = NULL;
   uint32_t          l2vic_int_vector;

   if (NULL == p_int_prop)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Interrut property pointer is NULL");
      return ADSP_EFAILED;
   }

   /* Clear the interrupt enabled flag */
   p_int_prop->is_interrupt_enabled = FALSE;

   /* Acquire a handle to DAL INTERRUPT CONTROLLER */
   if ((DAL_SUCCESS != (result = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &int_ctrl_hptr))) ||
       (NULL == int_ctrl_hptr))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Intr Reg: Failed to acquire a handle to INT Controller, result: 0x%lx", result);
      return ADSP_EFAILED;
   }

   /* Save the acquired handle */
   p_int_prop->int_ctr_hptr = int_ctrl_hptr;

   /* Query the interrupt to register with the string,
      if l2vic # not known / available */
   if (NULL != p_int_prop->p_intr_l2vic_string)
   {
      if (DAL_SUCCESS != DalInterruptController_GetInterruptID(int_ctrl_hptr,
                                                               p_int_prop->p_intr_l2vic_string,
                                                               (DALInterruptID *)&l2vic_int_vector))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPI GPIO INIT: Failed to query interrupt number");
         return ADSP_EFAILED;
      }

      /* Save the L2VIC number */
      p_int_prop->l2vic_int_vector = l2vic_int_vector;
   }

   /* Register with  Interrupt */
   if (DAL_SUCCESS != (result = DalInterruptController_RegisterISR(int_ctrl_hptr,                /* DAL Device handle */
                                                                   p_int_prop->l2vic_int_vector, /* L2VIC #*/
                                                                   (DALISR)p_int_prop->intr_cb,  /* Callback pointer */
                                                                   NULL,                         /* Callback funct arg */
                                                                   p_int_prop->trigger_type)))   /* Intr trigger type */
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to register ISR with l2vic[%lu], result: 0x%lx", p_int_prop->l2vic_int_vector, result);

      return ADSP_EFAILED;
   }

   /* Set the interrupt enabled flag */
   p_int_prop->is_interrupt_enabled = TRUE;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Interrupt registration successful, L2VIC[%lu]", p_int_prop->l2vic_int_vector);

   return ADSP_EOK;
}


ADSPResult afe_device_gated_intr_deregister(afe_dev_intr_prop_t *p_intr_prop)
{
   DALResult   dal_result = DAL_SUCCESS;
   ADSPResult  result = ADSP_EOK;

   if (NULL == p_intr_prop)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Interrut property pointer is NULL");
      return ADSP_EFAILED;
   }

   /* De-register the interrupt */
   if ((NULL != p_intr_prop->int_ctr_hptr) &&
       (TRUE == p_intr_prop->is_interrupt_enabled))
   {
      if (DAL_SUCCESS != (dal_result = DalInterruptController_Unregister(p_intr_prop->int_ctr_hptr,
                                                                         p_intr_prop->l2vic_int_vector)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to deregister interrupt, l2vic[%lu], result: 0x%lx",
               p_intr_prop->l2vic_int_vector, dal_result);

         result = ADSP_EFAILED;
      }
      else
      {
         p_intr_prop->is_interrupt_enabled = FALSE;
      }
   }

   /* Detach Interrupt Controller */
   if (NULL != p_intr_prop->int_ctr_hptr)
   {
      if (DAL_SUCCESS != (dal_result = DAL_DeviceDetach(p_intr_prop->int_ctr_hptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to detach from INT controller, result: 0x%lx", dal_result);
         result = ADSP_EFAILED;
      }
      else
      {
         p_intr_prop->int_ctr_hptr = NULL;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "Intr de-register completed successfully, l2vic_id: %lu", p_intr_prop->l2vic_int_vector);

   return result;
}


ADSPResult afe_device_pdc_gpio_init(gpio_int_properties_t *gpio_int_prop_ptr)
{
   DALResult            result = DAL_SUCCESS;
   DALGpioSignalType    pin_config;
   DalDeviceHandle      *tlmm_hptr = NULL;

   if (NULL == gpio_int_prop_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PDC GPIO INIT: gpio prop ptr is NULL");
      return ADSP_EFAILED;
   }

   /* Clear the GPIO config flag */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   /* Attach to DAL TLMM */
   if ((DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_hptr) != DAL_SUCCESS) || (tlmm_hptr == NULL))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PDC GPIO INIT: Failed to attach to TLMM");
      return ADSP_EFAILED;
   }

   /* Save the DAL TLMM handle */
   gpio_int_prop_ptr->tlmm_hptr = tlmm_hptr;

   /* Configure the GPIO propoerties. Currently set for codec interrupt.
      For generality, these values should be passed as input arguement to
      ths function. */

   pin_config = DAL_GPIO_CFG(gpio_int_prop_ptr->n_gpio,
                             0, /* Function Select */
                             DAL_GPIO_INPUT,
                             DAL_GPIO_NO_PULL,
                             DAL_GPIO_2MA);

   /* Call the DAL TLMM DAL to configure the GPIO */
   if (DAL_SUCCESS != (result = DalTlmm_ConfigGpio(tlmm_hptr,
                                                   pin_config,
                                                   DAL_TLMM_GPIO_ENABLE)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "PDC GPIO INIT: Failed to configure GPIO Pin[%lu], result: 0x%lx",
            gpio_int_prop_ptr->n_gpio, result);

      return ADSP_EFAILED;
   }

   /* Set the GPIO config flag to TRUE */
   gpio_int_prop_ptr->is_gpio_configured = TRUE;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "PDC GPIO INIT: Successfully configured GPIO Pin[%lu]", gpio_int_prop_ptr->n_gpio);

   return ADSP_EOK;
}

ADSPResult afe_device_pdc_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr)
{
   DALResult   dal_result = DAL_SUCCESS;
   ADSPResult  result = ADSP_EOK;

   if (NULL == gpio_int_prop_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PDC GPIO DEINIT: gpio prop ptr is NULL");
      return ADSP_EFAILED;
   }

   /* Detach TLMM */
   if (NULL != gpio_int_prop_ptr->tlmm_hptr)
   {
      if (DAL_SUCCESS != (dal_result = DAL_DeviceDetach(gpio_int_prop_ptr->tlmm_hptr)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PDC GPIO DEINIT: Failed to detach from TLMM");
         result = ADSP_EFAILED;
      }
      else
      {
         /* Clear the DAL device handle for TLMM */
         gpio_int_prop_ptr->tlmm_hptr = NULL;
      }
   }

   /* Clear the GPIO config flag */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "PDC GPIO DEINIT: GPIO deinit completed successfully");

   return result;
}

ADSPResult afe_device_pdc_gpio_intr_reg(gpio_int_properties_t *gpio_int_prop_ptr)
{
   DALResult            result = DAL_SUCCESS;
   DalDeviceHandle      *gpio_int_hptr = NULL;

   if (NULL == gpio_int_prop_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO INT prop ptr is NULL");
      return ADSP_EFAILED;
   }

   /* Clear the interrupt enabled flag */
   gpio_int_prop_ptr->intr_prop.is_interrupt_enabled = FALSE;

   /* Acquire a handle to GPIOInt */
   if ((DAL_DeviceAttach(DALDEVICEID_GPIOINT, &gpio_int_hptr) != DAL_SUCCESS) || (gpio_int_hptr == NULL))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PDC GPIO INIT: Failed to acquire a handle to GPIOInt");
      return ADSP_EFAILED;
   }

   /* Save the DAL GPIOInt handle */
   gpio_int_prop_ptr->gpio_int_hptr = gpio_int_hptr;

   /* Register ISR. For hoya arch chipsets, external interrupts like codec interrupt
      are routed via audio PDC mux within LPASS. Below API call takes care of
      required mux configuration and L2VIC configuration.
      Note that l2vic # is dynamically assigned based on the available interrupt line
      at PDC mux output. */
   if (DAL_SUCCESS != (result = GPIOInt_RegisterIsr(gpio_int_prop_ptr->gpio_int_hptr,
                                                    gpio_int_prop_ptr->n_gpio,
                                                    (GPIOIntTriggerType)gpio_int_prop_ptr->intr_prop.trigger_type,
                                                    (GPIOINTISR)gpio_int_prop_ptr->intr_prop.intr_cb,
                                                    0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "PDC GPIO INIT: Failed to configure GPIO Pin[%lu], result: 0x%lx",
            gpio_int_prop_ptr->n_gpio, result);

      return ADSP_EFAILED;
   }

   /* Set the interrupt enabled flag */
   gpio_int_prop_ptr->intr_prop.is_interrupt_enabled = TRUE;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "Configured PDC GPIO[%lu] Interrupt successfully", gpio_int_prop_ptr->n_gpio);

   return ADSP_EOK;
}

ADSPResult afe_device_pdc_gpio_intr_dereg(gpio_int_properties_t *gpio_int_prop_ptr)
{
   DALResult   dal_result = DAL_SUCCESS;
   ADSPResult  result = ADSP_EOK;

   if ((NULL == gpio_int_prop_ptr) ||
       (NULL == gpio_int_prop_ptr->gpio_int_hptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "GPIO prop ptr is NULL or DAL GPIOInit handle is NULL/not attached");

      return ADSP_EFAILED;
   }

   /* Dergister ISR */
   if (TRUE == gpio_int_prop_ptr->intr_prop.is_interrupt_enabled)
   {
      if (DAL_SUCCESS != (dal_result = GPIOInt_DeregisterIsr(gpio_int_prop_ptr->gpio_int_hptr,
                                                             gpio_int_prop_ptr->n_gpio,
                                                             (GPIOINTISR)gpio_int_prop_ptr->intr_prop.intr_cb)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to disable interrupt for GPIO[%lu], result: 0x%lx",
               gpio_int_prop_ptr->n_gpio, dal_result);

         result = ADSP_EFAILED;
      }
   }

   /* Detach GPIOInt */
   if (NULL != gpio_int_prop_ptr->gpio_int_hptr)
   {
      if (DAL_SUCCESS != (dal_result = DAL_DeviceDetach(gpio_int_prop_ptr->gpio_int_hptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "PDC GPIO DEINIT:Failed to detach from GPIOInt, result: 0x%lx", dal_result);

         result = ADSP_EFAILED;
      }
      else
      {
         /* Clear the DAL device handle for GPIOInt */
         gpio_int_prop_ptr->gpio_int_hptr = NULL;
      }
   }

   /* Clear the interrupt enabled flag */
   gpio_int_prop_ptr->intr_prop.is_interrupt_enabled = FALSE;

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "PDC GPIO[%lu] Interrupt de-registration successful",
         gpio_int_prop_ptr->n_gpio);

   return result;
}

