/** 
  @file  EFIPmicGpio.h
  @brief PMIC GPIO EFI Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/09/16   al      Updating GUID
 10/30/13   ajs     (Tech Pubs) Edited/added Doxygen comments and markup.
 06/18/13   al      Adding API for enabling/disabling dtest line for dig in
 02/27/13   al      Adding IRQ related APIs 
 01/22/13   al      Added real time interrupt status
 10/16/12   al	    Fixed out buffer drive strength enum
 08/14/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 07/27/12   al	    Commented out not regquired for 8974 and compiling for 8960
 05/03/12   vishalo Merge in Techpubs Doxygen change
 04/19/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/26/12   vishalo Merge in Techpubs Doxygen change
 11/29/11   sm      Added GpioGetStatus API
 11/02/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/11/11   dy      created

=============================================================================*/

#ifndef __EFIPMICGPIO_H__
#define __EFIPMICGPIO_H__


/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "PmicIrq.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicGpio_constants 
@{ */
/**
  Protocol version.
*/
#define PMIC_GPIO_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_pmicGpio_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicGpio_protocol */
#define EFI_PMIC_GPIO_PROTOCOL_GUID \
	{ 0x9ae91844, 0xef7d, 0x49ca, { 0x90, 0x34, 0x8, 0x22, 0xfb, 0x1a, 0x5d, 0x93 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC GPIO Protocol GUID.
*/
extern EFI_GUID gQcomPmicGpioProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_GPIO_PROTOCOL   EFI_QCOM_PMIC_GPIO_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicGpio_data_types 
@{ */
/** 
  GPIO type.
*/
typedef enum
{
  EFI_PM_GPIO_1,  /**< GPIO 1. */
  EFI_PM_GPIO_2,  /**< GPIO 2. */
  EFI_PM_GPIO_3,  /**< GPIO 3. */
  EFI_PM_GPIO_4,  /**< GPIO 4. */
  EFI_PM_GPIO_5,  /**< GPIO 5. */
  EFI_PM_GPIO_6,  /**< GPIO 6. */
  EFI_PM_GPIO_7,  /**< GPIO 7. */
  EFI_PM_GPIO_8,  /**< GPIO 8. */
  EFI_PM_GPIO_9,  /**< GPIO 9. */
  EFI_PM_GPIO_10, /**< GPIO 10. */
  EFI_PM_GPIO_11, /**< GPIO 11. */
  EFI_PM_GPIO_12, /**< GPIO 12. */
  EFI_PM_GPIO_13, /**< GPIO 13. */
  EFI_PM_GPIO_14, /**< GPIO 14. */
  EFI_PM_GPIO_15, /**< GPIO 15. */
  EFI_PM_GPIO_16, /**< GPIO 16. */
  EFI_PM_GPIO_17, /**< GPIO 17. */
  EFI_PM_GPIO_18, /**< GPIO 18. */
  EFI_PM_GPIO_19, /**< GPIO 19. */
  EFI_PM_GPIO_20, /**< GPIO 20. */
  EFI_PM_GPIO_21, /**< GPIO 21. */
  EFI_PM_GPIO_22, /**< GPIO 22. */
  EFI_PM_GPIO_23, /**< GPIO 23. */
  EFI_PM_GPIO_24, /**< GPIO 24. */
  EFI_PM_GPIO_25, /**< GPIO 25. */
  EFI_PM_GPIO_26, /**< GPIO 26. */
  EFI_PM_GPIO_27, /**< GPIO 27. */
  EFI_PM_GPIO_28, /**< GPIO 28. */
  EFI_PM_GPIO_29, /**< GPIO 29. */
  EFI_PM_GPIO_30, /**< GPIO 30. */
  EFI_PM_GPIO_31, /**< GPIO 31. */
  EFI_PM_GPIO_32, /**< GPIO 32. */
  EFI_PM_GPIO_33, /**< GPIO 33. */
  EFI_PM_GPIO_34, /**< GPIO 34. */
  EFI_PM_GPIO_35, /**< GPIO 35. */
  EFI_PM_GPIO_36, /**< GPIO 36. */
  EFI_PM_GPIO_37, /**< GPIO 37. */
  EFI_PM_GPIO_38, /**< GPIO 38. */
  EFI_PM_GPIO_39, /**< GPIO 39. */
  EFI_PM_GPIO_40, /**< GPIO 40. */
  EFI_PM_GPIO_41, /**< GPIO 41. */
  EFI_PM_GPIO_42, /**< GPIO 42. */
  EFI_PM_GPIO_43, /**< GPIO 43. */
  EFI_PM_GPIO_44  /**< GPIO 44. */
}EFI_PM_GPIO_WHICH_TYPE;


/** 
  Voltage source.
*/
typedef enum
{
  EFI_PM_GPIO_VIN0, /**< VIN0. */
  EFI_PM_GPIO_VIN1, /**< VIN1. */
  EFI_PM_GPIO_VIN2, /**< VIN2. */
  EFI_PM_GPIO_VIN3, /**< VIN3. */
  EFI_PM_GPIO_VIN4, /**< VIN4. */
  EFI_PM_GPIO_VIN5, /**< VIN5. */
  EFI_PM_GPIO_VIN6, /**< VIN6. */
  EFI_PM_GPIO_VIN7, /**< VIN7. */
  EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID /**< Invalid source. */
}EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE;


/** 
  Output buffer configuration.
*/
typedef enum
{
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_CMOS,             /**< CMOS output. */
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_NMOS,  /**< Open drain NMOS output. */
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS,  /**< Open drain PMOS output. */
    EFI_PM_GPIO_OUT_BUFFER_CONFIG__INVALID
}EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE;

/*! \enum pm_gpio_current_src_pulls_type
 *  \brief select current source pulls type
*/
typedef enum
{
    EFI_PM_GPIO_I_SOURCE_PULL_UP_30uA,     
  /**< Pull up is 30 microampere. */
    EFI_PM_GPIO_I_SOURCE_PULL_UP_1_5uA,    
  /**< Pull up is 1.5 microampere. */
    EFI_PM_GPIO_I_SOURCE_PULL_UP_31_5uA,   
  /**< Pull up is 31.5 microampere. */
    EFI_PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
  /**< Pull up is 1.5 microampere plus 30 microampere boost. */
    EFI_PM_GPIO_I_SOURCE_PULL_DOWN_10uA,   
  /**< Pull down is 10 microampere. */
    EFI_PM_GPIO_I_SOURCE_PULL_NO_PULL,     
    /**< No pull. */
    EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID
  /**< Invalid pull. @newpage */
}EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE;


/** 
  Enable/disable external pin output inversion.
*/
typedef enum
{
  EFI_PM_GPIO_INVERT_EXT_PIN_OUTPUT_DISABLE,
  /**< Disabled. */
  EFI_PM_GPIO_INVERT_EXT_PIN_OUTPUT_ENABLE,
  /**< Enabled. */
  EFI_PM_GPIO_INVERT_EXT_PIN_OUTPUT_INVALID
  /**< Invalid external pin output inversion. */
}EFI_PM_GPIO_INVERT_EXT_PIN_TYPE;


/** 
  Output buffer strength.
*/
typedef enum
{
  EFI_PM_GPIO_OUT_BUFFER_OFF,    /**< OFF. */
  EFI_PM_GPIO_OUT_BUFFER_LOW,    /**< Low. */
  EFI_PM_GPIO_OUT_BUFFER_MEDIUM, /**< Medium. */
  EFI_PM_GPIO_OUT_BUFFER_HIGH,   /**< High. */
  EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID /**< Invalid strength. */
}EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE;





/*! \enum pm_gpio_src_config_type
 *  \brief Source select
*/
typedef enum
{
    EFI_PM_GPIO_SOURCE_GND,                  /**< Ground. */
    EFI_PM_GPIO_SOURCE_PAIRED_GPIO,          /**< Paired GPIO. */
    EFI_PM_GPIO_SOURCE_SPECIAL_FUNCTION1,    /**< Special function 1. */
    EFI_PM_GPIO_SOURCE_SPECIAL_FUNCTION2,    /**< Special function 2. */
    EFI_PM_GPIO_SOURCE_SPECIAL_FUNCTION3,    /**< Special Function 3. */
    EFI_PM_GPIO_SOURCE_SPECIAL_FUNCTION4,    /**< Special Function 4. */  
    EFI_PM_GPIO_SOURCE_DTEST1,               /**< D-test 1. */
    EFI_PM_GPIO_SOURCE_DTEST2,               /**< D-test 2. */
    EFI_PM_GPIO_SOURCE_DTEST3,               /**< D-test 3. */
    EFI_PM_GPIO_SOURCE_DTEST4,               /**< D-test 4. */
    EFI_PM_GPIO_SOURCE_CONFIG_TYPE__INVALID
}EFI_PM_GPIO_SOURCE_CONFIG_TYPE;

/**
  Output modes.
*/
typedef enum
{

    EFI_PM_GPIO_INPUT_ON,           
    /**< GPIO is configured as input */
    EFI_PM_GPIO_INPUT_OUTPUT_ON,    
    /**< GGPIO is configured as input and output */
    EFI_PM_GPIO_OUTPUT_ON,          
    /**< GPIO is configured as output */
    EFI_PM_GPIO_ANA_PASS_THRU,      
    /**< GPIO is configured as Analog Pass Through */   
    EFI_PM_GPIO_INPUT_OUTPUT_OFF    
    /**< Both input and output are off */
} EFI_PM_GPIO_MODE_SELECT_TYPE;


/**
  Enable/disable DTEST buffer.
*/

typedef enum
{
    EFI_PM_GPIO_DTEST_DISABLE,                     /**< Disabled. */
    EFI_PM_GPIO_DTEST_ENABLE,                      /**< Enabled. */
    EFI_PM_GPIO_DTEST_BUFFER_ONOFF_TYPE__INVALID  /**< Invalid buffer. */
}EFI_PM_GPIO_DTEST_BUFFER_ONOFF_TYPE;




/** External pin configuration.
*/
typedef enum
{
    EFI_PM_GPIO_EXT_PIN_DISABLE,              /**< Disable EXT_PIN. */
    EFI_PM_GPIO_EXT_PIN_ENABLE,               /**< Puts EXT_PIN at high Z state and disables the block. */
    EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID  /*INVALID*/  
}EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE;

/**
  DTEST line for GPIO digital input control.
*/
typedef enum
{
    EFI_PM_GPIO_DIG_IN_DTEST1,             /**< DTEST1. */
    EFI_PM_GPIO_DIG_IN_DTEST2,             /**< DTEST2. */
    EFI_PM_GPIO_DIG_IN_DTEST3,             /**< DTEST3. */
    EFI_PM_GPIO_DIG_IN_DTEST4,             /**< DTEST4. */
    EFI_PM_GPIO_DIG_IN_DTEST_TYPE__INVALID /**< Invalid line. */
}EFI_PM_GPIO_DIG_IN_TYPE;


/**
  GPIO status.
*/
typedef struct 
{
    EFI_PM_GPIO_MODE_SELECT_TYPE                GpioModeSelect;
    /**< Output mode; see #EFI_PM_GPIO_MODE_SELECT_TYPE for details. */
    EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE             GpioVoltage_source;
    /**< Voltage source; see #EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE for details. */
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE          GpioOutBufferConfig;
    /**< Output buffer configuration; see #EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE 
         for details. */
    EFI_PM_GPIO_INVERT_EXT_PIN_TYPE             GpioInvertExtPin; 
    /**< External pin output inversion setting; 
         see #EFI_PM_GPIO_INVERT_EXT_PIN_TYPE for details. */
	EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE  GpioOutBufferDriveStrength;
    /**< Output buffer strength; 
         see #EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE for details. */
    EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE       GpioCurrentSourcePulls;
    /**< Current source pull; see #EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE 
         for details. */
    EFI_PM_GPIO_SOURCE_CONFIG_TYPE              GpioSourceConfig;
    /**< Source configuration; see #EFI_PM_GPIO_SOURCE_CONFIG_TYPE for 
         details. */
    EFI_PM_GPIO_DTEST_BUFFER_ONOFF_TYPE         GpioDtestBufferOnOff;
    /**< Enable DTEST buffer; see #EFI_PM_GPIO_DTEST_BUFFER_ONOFF_TYPE for 
         details. */
    EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE             GpioExtPinConfig;
    /**< Enable external pin configuration; 
         see #EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE for details. */
} EFI_PM_GPIO_STATUS_TYPE;
/** @} */ /* end_addtogroup efi_pmicGpio_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_GPIO_CONFIG_DIGITAL_INPUT */ 
/** @ingroup efi_pmicGpio_digital_input
  @par Summary
  Configures a PMIC GPIO as a digital input. 

  @param[in] PmicDeviceIndex     Primary: 0. Secondary: 1.
  @param[in] Gpio                Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE for 
                                 details.
  @param[in] ISourcePulls        Current source pull; see 
                                 #EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE for 
                                 details.
  @param[in] VoltageSource       Voltage source; see 
                                 #EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE for details.
  @param[in] OutBufferStrength   Output buffer strength; see 
                                 #EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE 
                                 for details.
  @param[in] Source              Source configuration; see 
                                 #EFI_PM_GPIO_SOURCE_CONFIG_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_CONFIG_DIGITAL_INPUT)(
  IN UINT32                                     PmicDeviceIndex,
  IN EFI_PM_GPIO_WHICH_TYPE                     Gpio,
  IN EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE      ISourcePulls,
  IN EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE            VoltageSource,
  IN EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE OutBufferStrength,
  IN EFI_PM_GPIO_SOURCE_CONFIG_TYPE             Source
);


/* EFI_PM_GPIO_CONFIG_DIGITAL_OUTPUT */ 
/** @ingroup efi_pmicGpio_digital_output
  @par Summary
  Configures a PMIC GPIO as a digital output.

  @param[in]  PmicDeviceIndex    Primary: 0. Secondary: 1.
  @param[in]  Gpio               Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE for 
                                 details.
  @param[in]  OutBufferConfig    Output buffer configuration; see 
                                 #EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE for 
                                 details.  
  @param[in]  VoltageSource      Voltage source; see 
                                 #EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE for details.
  @param[in]  Source             Source configuration; see 
                                 #EFI_PM_GPIO_SOURCE_CONFIG_TYPE for details.
  @param[in]  OutBufferStrength  Output buffer strength; see 
                                 #EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE 
                                 for details.  
  @param[in]  OutInversion       Whether the output is inverted.
  
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_CONFIG_DIGITAL_OUTPUT)(
  IN UINT32                                     PmicDeviceIndex,
  IN EFI_PM_GPIO_WHICH_TYPE                     Gpio,
  IN EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE         OutBufferConfig,
  IN EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE            VoltageSource,
  IN EFI_PM_GPIO_SOURCE_CONFIG_TYPE             Source,
  IN EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE OutBufferStrength,
  IN BOOLEAN                                    OutInversion
);


/* EFI_PM_GPIO_GET_STATUS */ 
/** @ingroup efi_pmicGpio_get_status
  @par Summary
  Gets the GPIO status.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE for 
                          details.
  @param[out] GpioStatus  GPIO status; see #EFI_PM_GPIO_STATUS_TYPE for 
                          details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_GET_STATUS)(
  IN  UINT32                    PmicDeviceIndex,
  IN  EFI_PM_GPIO_WHICH_TYPE    Gpio,
  OUT EFI_PM_GPIO_STATUS_TYPE   *GpioStatus
);


/* EFI_PM_GPIO_IRQ_ENABLE */ 
/** @ingroup efi_pmicGpio_irq_enable
  @par Summary
  Enables/disables IRQ for a GPIO.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.
  @param[in]  Enable           If TRUE, enable the interrupt for GPIO; 
                               if FALSE, disable the interrupt for GPIO.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_IRQ_ENABLE)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  BOOLEAN                    Enable
);


/* EFI_PM_GPIO_IRQ_CLEAR */ 
/** @ingroup efi_pmicGpio_irq_clear
  @par Summary
  Clears the GPIO IRQ.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_IRQ_CLEAR)(
 IN  UINT32                    PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE    Gpio
);


/* EFI_PM_GPIO_IRQ_SET_TRIGGER */ 
/** @ingroup efi_pmicGpio_irq_set_trigger
  @par Summary
  Configures the IRQ trigger for a GPIO.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.
  @param[in]  Trigger          IRQ trigger type. Values: \n
                               - EFI_PM_IRQ_TRIGGER_ACTIVE_LOW -- 
                                 Active low
                               - EFI_PM_IRQ_TRIGGER_ACTIVE_HIGH -- 
                                 Active high
                               - EFI_PM_IRQ_TRIGGER_RISING_EDGE -- 
                                 Rising edge
                               - EFI_PM_IRQ_TRIGGER_FALLING_EDGE -- 
                                 Falling edge
                               - EFI_PM_IRQ_TRIGGER_DUAL_EDGE -- 
                                 Dual edge
                               - EFI_PM_IRQ_TRIGGER_INVALID -- 
                                 Invalid trigger @vertspace{-13}

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_IRQ_SET_TRIGGER)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  EFI_PM_IRQ_TRIGGER_TYPE    Trigger
);


/* EFI_PM_GPIO_IRQ_STATUS */ 
/** @ingroup efi_pmicGpio_irq_status
  @par Summary
  Gets the GPIO IRQ status.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.
  @param[in]  Type             IRQ status to read. Values: \n
                               - EFI_PM_IRQ_STATUS_RT -- Real-time
                               - EFI_PM_IRQ_STATUS_LATCHED -- Latched
                               - EFI_PM_IRQ_STATUS_PENDING -- Pending
                               - EFI_PM_IRQ_STATUS_INVALID -- Invalid status
                               @vertspace{-13}
  @param[out] Status           IRQ status.


  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_IRQ_STATUS)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  EFI_PM_IRQ_STATUS_TYPE     Type,
 OUT BOOLEAN                    *Status
);


/* EFI_PM_GPIO_SET_DIG_IN_CTL */ 
/** @ingroup efi_pmicGpio_set_dig_in_ctl
  @par Summary
  Enables/disables a DTEST line for digital input control.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.
  @param[in]  DigIn            DTEST line for GPIO; see 
                               #EFI_PM_GPIO_DIG_IN_TYPE for details.
  @param[in]  Enable           If TRUE, enable the corresponding interrupt for
                               GPIO; \n
                               if FALSE, disable the corresponding interrupt
                               for GPIO.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_SET_DIG_IN_CTL)(
 IN  UINT32                           PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE           Gpio,
 IN  EFI_PM_GPIO_DIG_IN_TYPE          DigIn,
 IN  BOOLEAN                          Enable
);


/* EFI_PM_GPIO_EXT_PIN_CONFIG */ 
/** @ingroup efi_pm_gpio_ext_pin_config
  @par Summary
  Sets GPIO ext pin configuration.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Gpio             Which GPIO; see #EFI_PM_GPIO_WHICH_TYPE 
                               for details.
  @param[in]  ExtPinConfig     Ext pin config type. Refer #EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE for more info

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_GPIO_EXT_PIN_CONFIG)
(
 IN  UINT32                           PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE           Gpio,
 IN  EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE  ExtPinConfig
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicGpio_protocol
  @par Summary
  Qualcomm PMIC GPIO Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_gpio_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_GPIO_PROTOCOL {
  UINT64                                Revision;
  EFI_PM_GPIO_CONFIG_DIGITAL_INPUT      ConfigDigitalInput;
  EFI_PM_GPIO_CONFIG_DIGITAL_OUTPUT     ConfigDigitalOutput;
  EFI_PM_GPIO_GET_STATUS                GetStatus;
  EFI_PM_GPIO_IRQ_ENABLE                IrqEnable;
  EFI_PM_GPIO_IRQ_CLEAR                 IrqClear;
  EFI_PM_GPIO_IRQ_SET_TRIGGER           IrqSetTrigger;
  EFI_PM_GPIO_IRQ_STATUS                IrqStatus;
  EFI_PM_GPIO_SET_DIG_IN_CTL            SetDigInCtl;
  EFI_PM_GPIO_EXT_PIN_CONFIG            ExtPinConfig;
};

#endif	/* __EFIPMICGPIO_H__ */
