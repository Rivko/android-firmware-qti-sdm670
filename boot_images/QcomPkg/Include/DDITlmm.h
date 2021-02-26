#ifndef __DALITLMM_H__
#define __DALITLMM_H__
/**
  @file DDITlmm.h 
  
  Public interface include file for accessing the TLMM DAL device.
  
  The DDITlmm.h file is the public API interface to the Top Level Mode Multiplexor 
  (TLMM) Device Access Library (DAL).
*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/14   na      Remove configuration requests for remote processor (pre A-family)
07/05/10   sw      (Tech Pubs) edited Doxygen comments and markup; modified
                   Doxygen markup to meet Living Docs standards.
06/17/09   dcf     Fix for embedded output.
02/23/09   dcf     Added comments and versioning.
=============================================================================
Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary and Confidential. 
===========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"


/*==========================================================================

                   PUBLIC DEFINITIONS FOR MODULE

==========================================================================*/

/** 
  @addtogroup const_macros
  @{ 
*/ 

/**
  Macro that defines the TLMM DAL version currently in use. 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DALTLMM_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)
#else
#define DALTLMM_INTERFACE_VERSION
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */


/** 
  Configuration version 1.1 support.
  To satisfy the demand for backward compatibility with regard to configuration 
  bitfield lengths, the DAL_GPIO_VERSION bit was introduced. This bit specifies 
  that the GPIO configuration conforms to interface version 1.1, which supports 
  wider bitfields. 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_VERSION 0x20000000
#else
#define DAL_GPIO_VERSION
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Main configuration macro. 
    
  This main configuration macro is used when configuring a GPIO. It 
  utilizes the DAL_GPIO_VERSION definition to ensure that configurations conform 
  to the wider field size.\n
  
  This macro creates a GPIO configuration that is passed into APIs requiring a 
  DALGpioSignalType parameter. 
    
  @param gpio  [in] -- GPIO number to program.
  @param func  [in] -- Function selection (0 for generic I/O).
  @param dir   [in] -- Direction (input or output).
  @param pull  [in] -- Pull value (up, down, or none).
  @param drive [in] -- Drive strength to program.
   
  @sa
  DalTlmm_ConfigGpio
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_CFG(gpio, func, dir, pull, drive) \
         (((gpio) & 0x3FF)<< 4 | \
          ((func) & 0xF)|        \
          ((dir)  & 0x1) << 14|  \
          ((pull) & 0x3) << 15|  \
          ((drive)& 0xF) << 17| DAL_GPIO_VERSION)
#else
#define DAL_GPIO_CFG(gpio, func, dir, pull, drive)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/**
  GPIO number extraction macro.
    
  This extraction macro is used to retrieve the GPIO number from a DAL_GPIO_CFG
  formatted configuration.
 
  @param config [in] -- DAL_GPIO_CFG format configuration.
 
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_NUMBER(config)    (((config) >> 4) & 0x3FF)
#else
#define DAL_GPIO_NUMBER(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Function select extraction macro. 
    
  Extraction macro used to retrieve the function select from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_FUNCTION(config)   ((config) & 0xF)
#else
#define DAL_GPIO_FUNCTION(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */
/** 
  Direction value extraction macro.
    
  Extraction macro used to retrieve the direction from a DAL_GPIO_CFG formatted 
  configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG 
*/ 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_DIRECTION(config) (((config) >> 14) & 0x1)
#else
#define DAL_GPIO_DIRECTION(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Pull value extraction macro.
    
  This extraction macro is used to retrieve the pull value from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_PULL(config)      (((config) >> 15) & 0x3)
#else
#define DAL_GPIO_PULL(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Drive strength extraction macro.
    
  Extraction macro used to retrieve the drive strength from a DAL_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_CFG 
*/ 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_DRIVE(config)     (((config) >> 17) & 0xF)
#else
#define DAL_GPIO_DRIVE(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Invalid GPIO number marker.
    
  This marker specifies a GPIO as invalid. It can be useful to mark a GPIO as 
  invalid in the sleep table so that low-power reconfiguration of a particular 
  GPIO is bypassed during MSM sleep.
  
  This definition can be used in place of the GPIO number in the 
  DAL_GPIO_CFG macro. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration.
    
  @sa
  DAL_GPIO_NUMBER, DAL_GPIO_CFG 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_INVALID_PIN  0x3FF
#else
#define DAL_GPIO_INVALID_PIN
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */


/** 
  Special case configuration macro with an embedded output specification. 
    
  This is a special case GPIO macro that allows an output value to be embedded 
  in the configuration. The specified output value is written directly after 
  the GPIO is configured. This is typically used when specifying an inactive GPIO 
  configuration where the low-power output value must be stored.
    
  @param gpio   [in] -- GPIO number to program.
  @param func   [in] -- Function select (0 for generic I/O).
  @param dir    [in] -- Direction (input or output).
  @param pull   [in] -- Pull value (up, down, or none).
  @param drive  [in] -- Drive strength to program.
  @param outval [in] -- DALGpioValueType value (HIGH or LOW).
    
  @sa
  DALGpioValueType, DAL_GPIO_CFG
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval) \
          (DAL_GPIO_CFG((gpio), (func), (dir), (pull), (drive))\
          | (((outval)|0x2) << 0x15))
#else
#define DAL_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  Extraction macro to retrieve the output value of a GPIO.
    
  This macro returns the output value of a particular GPIO configuration when 
  either internally set or set by DAL_GPIO_CFG_OUT. This macro is not associated 
  with any value written to the hardware. Its sole purpose is to extract an 
  output value from a configuration. It is typically used when modifying an 
  inactive GPIO configuration. 
    
  @param config [in] -- DAL_GPIO_CFG format configuration,
   
  @sa
  DAL_GPIO_CFG_OUT 
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DAL_GPIO_GET_OUTVAL(config) (((config) >> 0x15) & 0x1)
#else
#define DAL_GPIO_GET_OUTVAL(config)
#endif /* end DOXYGEN_SHOULD_SKIP_THIS */

/** 
  @}
*/ /* end_group const_macro */

/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/



/** 
  @addtogroup dir_enums 
  @{ 
*/ 

/** 
  State of a GPIO.
    
  The DALGpioStatusType enumerations are used to tell if a particular GPIO is 
  configured to an active or inactive state. 
*/

typedef enum
{
  DAL_GPIO_INACTIVE = 0,  /**< -- Inactive state. */
  DAL_GPIO_ACTIVE   = 1,  /**< -- Active state. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioStatusType = 0x7fffffff
/** @endcond */
}DALGpioStatusType;


/** 
  GPIO ownership value.
    
  The DALGpioOwnerType enumerations are used to get the current processor owner 
  of a particular GPIO. 
*/
typedef enum
{
  DAL_GPIO_OWNER_MASTER,  /**< -- Secure Root Of Trust (sROT). */
  DAL_GPIO_OWNER_APPS1,    /**< -- Applications procesor 1. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioOwnerType = 0x7fffffff
/** @endcond */
}DALGpioOwnerType;


/** 
  Locking values for GPIO.
    
  The DALGpioLockType enumerations are rudimentary mechanisms provided for 
  debugging to ensure that a GPIO does not get changed, either by another driver 
  or by the internal TLMM driver until it is unlocked.
  
  @note This mechanism is strictly <i>use at your own risk</i>. Use it only for 
  debugging in cases where it is necessary. Do not use it as an overall 
  solution. 
*/

typedef enum
{
  DAL_GPIO_UNLOCKED = 0,    /**< -- Allow GPIO configuration overwrites. */
  DAL_GPIO_LOCKED   = 1,    /**< -- Deny configuration overwrites. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioLockType = 0x7fffffff
/** @endcond */

}DALGpioLockType;


/** 
  Locking mechanism clients.
    
  The DALGpioClientType is used in conjunction with the DALGpioLockType and 
  associated APIs to manipulate the locking mechanism of a GPIO.\n
  
  @note Locking a GPIO does not ensure that it cannot be unlocked by another 
  driver, so use it with extreme caution and only for debugging purposes.
*/
typedef enum
{
  DAL_GPIO_CLIENT_PWRDB = 0,   /**< -- Power debug client. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioClientType = 0x7fffffff
/** @endcond */

}DALGpioClientType;

/** 
  @}
*/ /*end_group dir_enums */


/*==========================================================================

                      PUBLIC GPIO CONFIGURATION MACROS

==========================================================================*/

/** 
  @addtogroup config_enums 
  @{ 
*/

/** 
  Direction enumerations for configuration. 
    
  The DALGpioDirectionType enumeration specifies the direction to use in the 
  configuration of a GPIO: an INPUT to the MSM or an OUTPUT from the MSM. 
    
  @sa
  DAL_GPIO_CFG
*/

typedef enum
{
  DAL_GPIO_INPUT  = 0,     /**< -- Set to INPUT. */
  DAL_GPIO_OUTPUT = 1,     /**< -- Set to OUTPUT. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioDirectionType = 0x7fffffff
/**@endcond */

}DALGpioDirectionType;



/** 
  Pull value for a GPIO configuration.
    
  The DALGpioPullType specifies what type of pull (if any) to use in the 
  configuration of a particular GPIO. 
    
  @sa
  DAL_GPIO_CFG 
*/

typedef enum
{
  DAL_GPIO_NO_PULL    = 0,   /**< -- Do not specify a pull. */
  DAL_GPIO_PULL_DOWN  = 0x1, /**< -- Pull the GPIO down. */
  DAL_GPIO_KEEPER     = 0x2, /**< -- Designate as a Keeper. */
  DAL_GPIO_PULL_UP    = 0x3, /**< -- Pull the GPIO up. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioPullType = 0x7fffffff
/** @endcond */

}DALGpioPullType;


/** 
  Drive strength values 
    
  The DALGpioDriveType enumeration specifies the drive strength to use in the 
  configuration of a GPIO. 
    
  @sa
  DAL_GPIO_CFG 
*/

typedef enum
{
  DAL_GPIO_2MA     = 0,    /**< -- Specify a 2 mA drive. */
  DAL_GPIO_4MA     = 0x1,  /**< -- Specify a 4 mA drive. */
  DAL_GPIO_6MA     = 0x2,  /**< -- Specify a 6 mA drive. */
  DAL_GPIO_8MA     = 0x3,  /**< -- Specify an 8 mA drive. */
  DAL_GPIO_10MA    = 0x4,  /**< -- Specify a 10 mA drive. */
  DAL_GPIO_12MA    = 0x5,  /**< -- Specify a 12 mA drive. */
  DAL_GPIO_14MA    = 0x6,  /**< -- Specify a 14 mA drive. */
  DAL_GPIO_16MA    = 0x7,  /**< -- Specify a 16 mA drive. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioDriveType = 0x7fffffff
/** @endcond */

}DALGpioDriveType;


/**
  Output value specification for general purpose I/O.
    
  The DALGpioValueType specifies the value to write to GPIO lines configured as 
  OUTPUT with function select 0 (also known as <i>bit banging</i>).
    
  @sa
  DAL_GPIO_CFG_OUT, DalTlmm_GpioOut, DalTlmm_GpioOutGroup
*/

typedef enum
{
  DAL_GPIO_LOW_VALUE,     /**< -- Drive the output LOW. */
  DAL_GPIO_HIGH_VALUE,    /**< -- Drive the output HIGH. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioValueType = 0x7fffffff
/** @endcond */

}DALGpioValueType;


/** 
  Signal Type alias.
    
  The DALGpioSignalType is an alias for a 32-bit configuration value. The type 
  name can be used by any calling drivers, but the underlying configuration is 
  used as an unsigned 32-bit integer.
    
  @sa
  DAL_GPIO_CFG 
*/


/** @cond */
#define DAL_PLACEHOLDER_DALGpioSignalType 0x7fffffff
/** @endcond */

typedef uint32 DALGpioSignalType;


/** 
  Low-power subset enumerations.
    
  The DALGpioSleepConfigType defines the subset of GPIOs to configure to 
  low-power states during power collapse or TCXO shutdown.
  
  @note These enumerations must be used only by the proper power drivers. They 
  are not for generic use because they can affect every GPIO in the system.
    
  @sa
  DalTlmm_ConfigGpiosForSleep, DalTlmm_RestoreGpiosFromSleep
*/

typedef enum
{
  DAL_GPIO_SLEEP_CONFIG_ALL,   /**< -- Handle all GPIOs. */
  DAL_GPIO_SLEEP_CONFIG_APPS,  /**< -- Handle the application processor-owned GPIO. */
  DAL_GPIO_SLEEP_CONFIG_NUM_MODES, /**< -- Number of sleep mode GPIO manipulators. */
/** @cond */
  DAL_PLACEHOLDER_DALGpioSleepConfigType = 0x7fffffff
/** @endcond */
}DALGpioSleepConfigType;


/** 
  Enable or disable a GPIO.
    
  The DALGpioEnableType enumerations specify whether a GPIO is to be configured 
  to an active (enable) or inactive (disable) state. 
    
  @sa
  DalTlmm_ConfigGpio, DalTlmm_ConfigGpioGroup 
*/

typedef enum 
{
  DAL_TLMM_GPIO_DISABLE,  /**< -- Use the internal inactive configuration. */
  DAL_TLMM_GPIO_ENABLE,   /**< -- Use the configuration passed as parameter. */

/** @cond */
  DAL_PLACEHOLDER_DALGpioEnableType = 0x7fffffff
/** @endcond */

}DALGpioEnableType;

/** 
  @}
*/ /* end_group config_enums */


/** 
  @addtogroup block_configs 
  @{ 
*/

/** 
  TLMM port selection enumerations.
    
  The DALGpioPortType is a port configuration enumeration for publicly available 
  TLMM ports, which abstract miscellaneous TLMM registers and are used with the 
  DalTlmm_SetPort API.
  
  @note Additions to this enumeration and associated functionality are added 
  on a needed basis, and not every enumeration is available or defined on every 
  target). 
    
  @sa
  DalTlmm_SetPort
*/

/* ====================================================================
   New Ports must be added to the end of this enumeration (just before 
   DAL_TLMM_NUM_PORTS).
=====================================================================*/
typedef enum
{
  DAL_TLMM_USB_PORT_SEL,         /**< -- USB port selection */
  DAL_TLMM_PORT_CONFIG_USB,      /**< -- Configuration port for USB. */
  DAL_TLMM_PORT_TSIF,            /**< -- Transport Stream InterFace port 
                                         selection. */
  DAL_TLMM_AUD_PCM,              /**< -- Audio configuration selector. */
  DAL_TLMM_UART1,                /**< -- UART 1 configuration selector. */
  DAL_TLMM_UART3,                /**< -- UART 3 configuration selector. */
  DAL_TLMM_LCDC_CFG,             /**< -- Display configuration for LCD. */
  DAL_TLMM_KEYSENSE_SC_IRQ,      /**< -- Keysense visibility on the Scorpion 
                                         processor. */
  DAL_TLMM_KEYSENSE_A9_IRQ,      /**< -- Keysense visibility on the 
                                         ARM9&reg;&nbsp; processor. */
  DAL_TLMM_TCXO_EN,              /**< -- TCXO enable functionality. */
  DAL_TLMM_SSBI_PMIC,            /**< -- PMIC SSBI pin configuration. */
  DAL_TLMM_PA_RANGE1,            /**< -- Power Amplifier Range 1 configuration. */
  DAL_TLMM_SPARE_WR_UART_SEL,    /**< -- UART selector configuration. */
  DAL_TLMM_PAD_ALT_FUNC_SDIO_EN, /**< -- SDIO function selector. */
  DAL_TLMM_SPARE_WR_TCXO_EN,     /**< -- Spare TCXO enable. */
  DAL_TLMM_SPARE_WR_PA_ON,       /**< -- Power Amplifier On selection. */
  DAL_TLMM_SDC3_CTL,             /**< -- Slot 3 for the Secure Digital Card. */
  DAL_TLMM_SDC4_CTL,             /**< -- Slot 4 for the Secure Digital Card.  */
  DAL_TLMM_UIM1_PAD_CTL,         /**< -- UIM1 PAD Control support. */
  DAL_TLMM_UIM2_PAD_CTL,         /**< -- UIM2 PAD Control support. */
  DAL_TLMM_PMIC_HDRV_CTL_CLK,    /**< -- TLMM PMIC HDRV pull control */
  DAL_TLMM_PMIC_HDRV_CTL_DATA,   /**< -- TLMM PMIC HDRV pull control */

/** @cond */
  DAL_TLMM_NUM_PORTS,
  DAL_PLACEHOLDER_DALGpioPortType = 0x7fffffff
/** @endcond */
}DALGpioPortType;

/** 
  @}
*/ /* end_group block_configs */


/*==========================================================================
  Function pointer table for TLMM glue code.  This is part of the DAL 
  framework glue code.
==========================================================================*/
/** @cond */
typedef struct DalTlmm DalTlmm;
struct DalTlmm
{
   struct DalDevice DalDevice;
   DALResult (*ConfigGpio)(DalDeviceHandle * _h, DALGpioSignalType gpio_config, DALGpioEnableType enable);
   DALResult (*ConfigGpioGroup)(DalDeviceHandle * _h, DALGpioEnableType enable, DALGpioSignalType* gpio_group, uint32 size);
   DALResult (*GpioIn)(DalDeviceHandle * _h, DALGpioSignalType gpio_config, DALGpioValueType*  value);
   DALResult (*GpioOut)(DalDeviceHandle * _h, DALGpioSignalType gpio_config, DALGpioValueType value);
   DALResult (*GpioOutGroup)(DalDeviceHandle * _h, DALGpioSignalType* gpio_config, uint32 size, DALGpioValueType value);
   DALResult (*SetTlmmPort)(DalDeviceHandle * _h, DALGpioPortType Port, uint32  value);
   DALResult (*GetCurrentConfig)(DalDeviceHandle * _h, uint32 gpio_number, DALGpioSignalType* config);
   DALResult (*SetInactiveConfig)(DalDeviceHandle * _h, uint32 gpio_number, DALGpioSignalType config);
   DALResult (*GetInactiveConfig)(DalDeviceHandle *h, uint32 gpio_number, DALGpioSignalType* config);
   DALResult (*GetGpioNumber)(DalDeviceHandle * _h, DALGpioSignalType gpio_config, uint32 *  number);
   DALResult (*GetGpioStatus)(DalDeviceHandle * _h, uint32 gpio_number, DALGpioStatusType* status);
   DALResult (*GetOutput)(DalDeviceHandle * _h, uint32 gpio_number, DALGpioValueType* value);
   DALResult (*PostInit)(DalDeviceHandle * _h);
};


typedef struct DalTlmmHandle DalTlmmHandle; 
struct DalTlmmHandle 
{
   uint32 dwDalHandleId;
   const DalTlmm * pVtbl;
   void * pClientCtxt;
};

#define DAL_TlmmDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTLMM_INTERFACE_VERSION,hDalDevice)

/** @endcond */



/*==========================================================================

                     APPLICATION PROGRAMMER'S INTERFACE

==========================================================================*/
/** 
  @addtogroup driver_apis
  @{
*/


/* ============================================================================
**  Function : DalTlmm_ConfigGpio
** ============================================================================*/
/**
  Configures a GPIO.
 
  This function configures a general purpose I/O or a hardware function 
  multiplexed on a particular GPIO pin. The configuration is based in part on 
  the value for the enable parameter.
  - If this parameter is set to DAL_TLMM_GPIO_ENABLE, the configuration 
    associated with the gpio_config parameter is programmed to hardware.
  - If DAL_TLMM_GPIO_DISABLE is specified, the GPIO number is extracted from 
    the gpio_config parameter and used to index an internal table to retrieve 
    the inactive (or low-power) configuration associated with the particular GPIO.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration to use whenn programming a GPIO.
  @param enable      [in] -- DAL_TLMM_GPIO_ENABLE or DAL_TLMM_GPIO_DISABLE.
    
  @return
  DAL_SUCCESS -- GPIO was successfully configured as requested.\n
  DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership 
  cannot be obtained from a secure root.
    
  @dependencies
  None.

  @sideeffects
  Ownership for the specified GPIO can be changed, if allowed 
  and required. This may be necessary to provide visibility to 
  generic I/O on certain multiprocessor systems. 
    
  @sa
  DAL_GPIO_CFG, DALGpioSignalType
*/ 

static __inline DALResult
DalTlmm_ConfigGpio(DalDeviceHandle * _h, DALGpioSignalType gpio_config, DALGpioEnableType enable)
{
   return ((DalTlmmHandle *)_h)->pVtbl->ConfigGpio( _h, gpio_config, enable);
}


/* ============================================================================
**  Function : DalTlmm_ConfigGpioGroup
** ============================================================================*/
/**
  Configures a group of GPIOs.
 
  This function configures a group of GPIOs or a hardware function multiplexed 
  on a particular GPIO pin.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param enable      [in] -- DAL_TLMM_GPIO_ENABLE or DAL_TLMM_GPIO_DISABLE.
  @param *gpio_group [in] -- Pointer to an array of GPIO configurations.
  @param size        [in] -- Number of GPIO configurations in gpio_group.
    
  @return
  If 1, DAL_SUCCESS -- All configurations were valid and any remote requests 
                       were successful.\n
  DAL_ERROR -- Invalid configurations; unsuccessful requests.
    
  @dependencies
  None.

  @sa
  DalTlmm_ConfigGpio
*/ 

static __inline DALResult
DalTlmm_ConfigGpioGroup
(
  DalDeviceHandle * _h, 
  DALGpioEnableType  enable,
  DALGpioSignalType* gpio_group, 
  uint32             size
)
{
   return 
     ((DalTlmmHandle *)_h)->pVtbl->ConfigGpioGroup( _h, enable, gpio_group, size);
}


/* ============================================================================
**  Function : DalTlmm_GpioIn
** ============================================================================*/
/**
  Reads a GPIO value.
 
  Reads the value of a GPIO that has been configured as an INPUT.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param *value     [out] -- Value to be returned to the caller.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GpioIn
(
  DalDeviceHandle *_h, 
  DALGpioSignalType gpio_config, 
  DALGpioValueType*value
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GpioIn( _h, gpio_config, value);
}


/* ============================================================================
**  Function : DalTlmm_GpioOut
** ============================================================================*/
/**
  Writes a GPIO value.
 
  Writes a value (high or low) to a GPIO that is configured as an OUTPUT, has a 
  function select set to 0, and has no pull. This type of configuration is known 
  as general purpose I/O or bit-banged I/O.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param value       [in] -- Value to write (DAL_GPIO_VALUE_HIGH or 
                             DAL_GPIO_VALUE_LOW).
    
  @return
  DAL_SUCCESS -- GPIO is valid.\n
  DAL_ERROR -- Invalid GPIO.

  @dependencies
  None.
    
  @sa
  DALGpioValueType
*/

static __inline DALResult
DalTlmm_GpioOut
(
  DalDeviceHandle  *_h, 
  DALGpioSignalType gpio_config, 
  DALGpioValueType  value
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GpioOut( _h, gpio_config, value);
}


/* ============================================================================
**  Function : DalTlmm_GpioOutGroup
** ============================================================================*/
/**
  Writes an output value to a group of GPIOs.
 
  Writes a value (high or low) to an array of GPIOs that are configured as OUTPUT,
  have function selects set to 0, and have pull set to no-pull.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param *gpio_group   [in] -- Array of GPIO configurations to which to write an
                               output value.
  @param size          [in] -- Number of elements in the gpio_group array.
  @param value         [in] -- Value to write (DAL_GPIO_VALUE_HIGH or
                               DAL_GPIO_VALUE_LOW).
    
  @return
  DAL_SUCCESS -- GPIO numbers are valid.\n
  DAL_ERROR -- Invalid numbers.

  @dependencies
  None.
    
  @sa
  DalTlmm_GpioOut, DALGpioValueType
*/

static __inline DALResult
DalTlmm_GpioOutGroup
(
  DalDeviceHandle   *_h, 
  DALGpioSignalType *gpio_group, 
  uint32            size, 
  DALGpioValueType  value
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GpioOutGroup( _h, 
                                                      gpio_group, 
                                                      size, value);
}


/* ============================================================================
**  Function : DalTlmm_SetPort
** ============================================================================*/
/**
  Configures a TLMM hardware port.
 
  This function configures the TLMM hardware based on the DALGpioPortType value 
  passed in as a parameter. This function is used to configure non-GPIO TLMM 
  registers.
 
  @param *_h   [in] -- Handle to the TLMM DAL device.
  @param port  [in] -- DALGpioPortType enumeration associated with the hardware 
                       functionality needed.
  @param value [in] -- Enable/disable value (0 = Disable, 1 = Enable).
    
  @return
  DAL_SUCCESS -- Operation was successful.\n
  DAL_ERROR -- Operation was unsuccessful.

  @dependencies
  None.
    
  @sa
  DALGpioPortType
*/

static __inline DALResult
DalTlmm_SetPort
(
  DalDeviceHandle *_h, 
  DALGpioPortType port,
  uint32          value
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->SetTlmmPort( _h, port, value);
}


/* ============================================================================
**  Function : DalTlmm_GetCurrentConfig
** ============================================================================*/
/**
  Retrieves the current configuration of a GPIO.
 
  This function retrieves the currently programmed configuration for a 
  particular GPIO.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration.
  @param *gpio_config [out] -- Current configuration.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetCurrentConfig
(
  DalDeviceHandle  *_h, 
  uint32           gpio_number, 
  DALGpioSignalType *gpio_config
)
{
   return 
     ((DalTlmmHandle *)_h)->pVtbl->GetCurrentConfig( _h, gpio_number, gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_SetInactiveConfig
** ============================================================================*/
/**
  Sets the inactive configuration of a GPIO.
 
  This function is used to modify the inactive or low-power configuration of a 
  GPIO. It modifies the configuration that is programmed when 
  DAL_TLMM_GPIO_DISABLE is passed in to the configuration functions.

  This function was originally provided for power debugging, but it can be used 
  elsewhere as necessary. It does not change the current configuration of a GPIO.
 
  @param *_h          [in] -- Handle to the TLMM DAL device.
  @param gpio_number  [in] -- GPIO number for which to retrieve the configuration.
  @param gpio_config [out] -- Configuration to use to replace the low-power
                              configuration in the table.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
    
  @sa
  DalTlmm_GetInactiveConfig
*/

static __inline DALResult
DalTlmm_SetInactiveConfig
(
  DalDeviceHandle   *_h, 
  uint32            gpio_number, 
  DALGpioSignalType gpio_config
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->SetInactiveConfig( _h, 
                                                           gpio_number, 
                                                           gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_GetInactiveConfig
** ============================================================================*/
/**
  Retrieves the inactive configuration of a GPIO.
 
  This function retrieves the currently set low-power (inactive) configuration 
  for a particular GPIO.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration
  @param *gpio_config [out] -- Currently set low-power configuration.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
    
  @sa
  DalTlmm_SetInactiveConfig
*/

static __inline DALResult
DalTlmm_GetInactiveConfig
(
  DalDeviceHandle  *_h, 
  uint32           gpio_number, 
  DALGpioSignalType *gpio_config
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetInactiveConfig( _h, 
                                                           gpio_number, 
                                                           gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioNumber
** ============================================================================*/
/**
  Retrieves a GPIO number.
 
  Retrieves the GPIO number from a specified configuration. This function does 
  the same thing as the macro DAL_GPIO_NUMBER.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_config   [in] -- Configuration from which to get the GPIO number.
  @param *gpio_number [out] -- GPIO number to return to the caller.
    
  @return
  Returns DAL_SUCCESS even if the GPIO number is invalid for the the current 
  target.
      
  @dependencies
  None.

  @sa
  DAL_GPIO_NUMBER
*/

static __inline DALResult
DalTlmm_GetGpioNumber
(
  DalDeviceHandle   *_h, 
  DALGpioSignalType gpio_config, 
  uint32            *gpio_number
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioNumber( _h, 
                                                     (DALGpioSignalType)gpio_config, 
                                                     gpio_number);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioStatus
** ============================================================================*/
/**
  Retrieves the current status of a GPIO.
 
  This function retrieves the current status of a GPIO: whether it is configured 
  to an active state or an inactive state.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_number [in] -- GPIO number for which to retrieve the configuration.
  @param *status    [out] -- State of the GPIO: either DAL_GPIO_ACTIVE or
                             DAL_GPIO_INACTIVE.
 
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetGpioStatus
(
  DalDeviceHandle   *_h, 
  uint32            gpio_number, 
  DALGpioStatusType *status
)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioStatus( _h, gpio_number, status);
}


/* ============================================================================
**  Function : DalTlmm_GetOutput
** ============================================================================*/
/**
  Retrieves the output value of a GPIO.
 
  This function retrieves the current output value of a GPIO that has been 
  configured as a generic I/O with output enabled.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_number [in] -- GPIO number to use to obtain the output value.
  @param *value     [out] -- Current value of the output GPIO (DAL_GPIO_VALUE_LOW 
                             or DAL_GPIO_VALUE_HIGH).
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetOutput(DalDeviceHandle * _h, uint32 gpio_number, DALGpioValueType* value)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetOutput( _h, gpio_number, value);
}


/* ============================================================================
**  Function : DalTlmm_PostInit
** ============================================================================*/
/**
  Performs any additional initialization necessary.
 
  This function performs any additional initialization that cannot be performed 
  during the first initialization of TLMM, such as shared memory.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
    
  @return
  DAL_SUCCESS -- Shared memory is initialized.\n
  DAL_ERROR -- Shared memory is not initialized.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_PostInit(DalDeviceHandle * _h)
{
   return ((DalTlmmHandle *)_h)->pVtbl->PostInit( _h);
}


/** 
  @}
*/ /* end_group driver_apis */

/** @cond */
#define DalTlmm_InterruptConfig DalTlmm_GpioBypassSleep
/** @endcond */

/* ============================================================================
**  Function : Tlmm_GpioIn
** ============================================================================*/
/**
  Non DAL GPIOIn Function.
 
  This function read the given GPIO status.
 
  @param eGpioConfig [in] -- GPIO configuration.
  @param eCValue [out]    -- GPIO status 0/1.
    
  @return
  DAL_SUCCESS / DAL_ERROR.
    
  @dependencies
  None.
*/

DALResult  Tlmm_GpioIn
( 
  DALGpioSignalType  eGpioConfig,  
  DALGpioValueType*  eValue
);


DALResult Tlmm_ConfigGpioGroup
(
  DALGpioEnableType  enable,
  DALGpioSignalType* gpio_group, 
  uint32             size
);

#endif
