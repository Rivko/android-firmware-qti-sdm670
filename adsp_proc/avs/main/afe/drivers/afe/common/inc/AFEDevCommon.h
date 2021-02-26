#ifndef __AFE_DEV_COMMON_H__
#define __AFE_DEV_COMMON_H__
/*========================================================================
  This header file contains common driver functions for AFE service

  Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEDevCommon.h#1 $
 ====================================================================== */

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEDevCommon.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/22/17    SJ      New file created

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "hwd_devcfg.h"
#include "DDIInterruptController.h"
#include "DDIGPIOInt.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDITlmm.h"

/*==========================================================================
  Macro definitions
  ========================================================================== */

#define AFE_GPIO_NUM_INVALID            0xFFFFFFFFUL


/* Callback function for interrupt callback */
typedef void (*intr_callback)(void *);

/*==========================================================================
  Structure definitions
  ========================================================================== */
/**
 * Struct for (DAL) Interrupt properties
 */
typedef struct afe_dev_intr_prop
{
   DalDeviceHandle       *int_ctr_hptr;         //handle to DAL interrupt controller
   uint32_t              l2vic_int_vector;      //L2VIC interrupt number
   bool_t                is_interrupt_enabled;  //Is interrupt enabled flag
   intr_callback         intr_cb;               //ISR Callback function pointer
   uint32_t              trigger_type;          //Interupt trigger type
   lpasshwio_irq_type_t  irq_type;              //Interrupt type
   char                  *p_intr_l2vic_string;  //String for this l2vic interrupt
} afe_dev_intr_prop_t;


/**
 * Struct for GPIO Interrupt properties
 */
typedef struct gpio_int_properties
{
   uint32_t              n_gpio;             // GPIO pin number
   DalDeviceHandle       *tlmm_hptr;         // Handle to the Top Level Mux Mode
                                             // ->Used to configure the GPIO pin
   DalDeviceHandle       *gpio_int_hptr;     // Handle to the GPIO Interrupt
                                             // ->Used to configure the interrupt
   lpasshwio_gpio_type_t gpio_type;          // GPIO type, chip level/LPI TLMM etc.                                            
   afe_dev_intr_prop_t   intr_prop;          // Interrupt property associated with
                                             //   this GPIO
   bool_t                is_gpio_configured; // Is GPIO configured

} gpio_int_properties_t;

/*==========================================================================
  Function definitions
  ========================================================================== */

/** 
   This function initializes the LPASS h/w info
   from device config 
 
   @param[in] None
   @param[out] None

   @result
   returns ADSP_EOK, if initialization success
   returns ADSP error code, if any error occurs

   @dependencies
   None.

*/
ADSPResult afe_dev_init_lpass_hw_info(void);

/** 
    This function is used to register the gated interrupt
    via LPI/PMU
 
   @param[in] afe_dev_intr_prop_t  :   p_int_prop pointer

   @result
   returns ADSP_EOK, if interrupt configuration is success
   returns ADSP_EFAILED, if any error occurs

   @dependencies
   None.

*/
ADSPResult afe_device_gated_intr_register(afe_dev_intr_prop_t *p_intr_prop);

/** 
   This function is used to de-register the gated interrupt 
   via PMU/LPI 
 
   @param[in] afe_dev_intr_prop_t  :   p_int_prop pointer 
 
   @result
   returns ADSP_EOK,     if interrupt de-reg is success 
   returns ADSP_EFAILED, if any error occurs

   @dependencies
   None.

*/
ADSPResult afe_device_gated_intr_deregister(afe_dev_intr_prop_t *p_intr_prop);

/** 
   This function is used to get the codec interrupt 
   type. 
 
   @param[in] None
 
   @result
   returns lpasshwio_irq_type_t : Codec interrupt type

   @dependencies
   None.

*/
lpasshwio_irq_type_t afe_get_cdc_intr_type(void);

/** 
   This function is used to get the codec interrupt 
   GPIO type. 
 
   @param[in] None
 
   @result
   returns lpasshwio_irq_type_t : Codec interrupt type

   @dependencies
   None.

*/
lpasshwio_gpio_type_t afe_get_cdc_intr_gpio_type(void);

/** 
   This function is used to get the GPIO num for 
   codec interrupt. GPIO number depends upon 
   codec interrupt type as default, PDC or LPI
 
   @param[in] None
 
   @result
   returns uint32_t : Codec interrupt GPIO number

   @dependencies
   None.

*/
uint32_t afe_get_cdc_intr_gpio_num(void);


/** 
   This function is used to configure the chip level 
   GPIO 
 
   @param[in] prop_string_ptr:   pointer to GPIO string 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
   @param[in] ext_gpio_num:  GPIO num provided by API caller to 
         override the pre-defined mapped GPIO for a given
         interrupt.
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_gpio_init(const char *prop_string_ptr,
                                gpio_int_properties_t *gpio_int_prop_ptr,
                                uint32_t ext_gpio_num);

/** 
   This function is used to de-init the chip level GPIO 
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
 
   @result
   returns ADSP_EOK, if de-init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr);

/** 
   This function is used to configure the chip level 
   GPIO with PDC mux config
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_pdc_gpio_init(gpio_int_properties_t *gpio_int_prop_ptr);

/** 
   This function is used to de-init the chip level 
   GPIO with PDC mux config
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_pdc_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr);

/** 
   This function is used to register interrupt for PDC gated 
   GPIO 
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO config struct 
         object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies: None

*/
ADSPResult afe_device_pdc_gpio_intr_reg(gpio_int_properties_t *gpio_int_prop_ptr);

/** 
   This function is used to de-register the interrupt for PDC 
   gated GPIO 
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO config struct
         object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_pdc_gpio_intr_dereg(gpio_int_properties_t *gpio_int_prop_ptr);


/** 
   This function is used to configure the LPI GPIO
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_lpi_gpio_init(gpio_int_properties_t *gpio_int_prop_ptr);

/** 
   This function is used to deinit the LPI GPIO
 
   @param[in] gpio_int_prop_ptr:   pointer to GPIO interrupt 
         prop struct object
 
   @result
   returns ADSP_EOK, if init success, else ADSP error code

   @dependencies
   None.

*/
ADSPResult afe_device_lpi_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr);


#ifdef __cplusplus

#endif //!SIM



#endif // !defined(__AFE_DEV_COMMON_H__)
