#ifndef __UTLMM_H__
#define __UTLMM_H__
/**
  @file uTlmm.h 
  
  Public interface include file for accessing the TLMM device.
  
  The uTlmm.h file is the public API interface to the Top Level Mode Multiplexor 
  (TLMM) for the micro sensors subsystem.
*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/uTlmm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/15   dcf     Initial verion.
=============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary and Confidential. 
===========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "com_dtypes.h"


/*==========================================================================

                   PUBLIC DEFINITIONS FOR MODULE

==========================================================================*/

/** 
  Main configuration macro. 
    
  This main configuration macro is used when configuring a GPIO.
  
  This macro creates a GPIO configuration that is passed into APIs requiring a 
  uTlmmGpioSignalType parameter. 
    
  @param gpio  [in] -- GPIO number to program.
  @param func  [in] -- Function selection (0 for generic I/O).
  @param dir   [in] -- Direction (input or output).
  @param pull  [in] -- Pull value (up, down, or none).
  @param drive [in] -- Drive strength to program.
   
  @sa
  uTlmm_ConfigGpio
*/

#define UTLMM_GPIO_CFG(gpio, func, dir, pull, drive) \
         (((gpio) & 0x3FF)<< 4 | \
          ((func) & 0xF)|        \
          ((dir)  & 0x1) << 14|  \
          ((pull) & 0x3) << 15|  \
          ((drive)& 0xF) << 17)

/**
  GPIO number extraction macro.
    
  This extraction macro is used to retrieve the GPIO number from a UTLMM_GPIO_CFG
  formatted configuration.
 
  @param config [in] -- UTLMM_GPIO_CFG format configuration.
 
  @sa
  UTLMM_GPIO_CFG
*/
#define UTLMM_GPIO_NUMBER(config)    (((config) >> 4) & 0x3FF)


/** 
  Function select extraction macro. 
    
  Extraction macro used to retrieve the function select from a UTLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- UTLMM_GPIO_CFG format configuration.
    
  @sa
  UTLMM_GPIO_CFG
*/
#define UTLMM_GPIO_FUNCTION(config)   ((config) & 0xF)


/** 
  Direction value extraction macro.
    
  Extraction macro used to retrieve the direction from a UTLMM_GPIO_CFG formatted 
  configuration. 
    
  @param config [in] -- UTLMM_GPIO_CFG format configuration.
    
  @sa
  UTLMM_GPIO_CFG 
*/ 
#define UTLMM_GPIO_DIRECTION(config) (((config) >> 14) & 0x1)


/** 
  Pull value extraction macro.
    
  This extraction macro is used to retrieve the pull value from a UTLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- UTLMM_GPIO_CFG format configuration.
    
  @sa
  UTLMM_GPIO_CFG
*/
#define UTLMM_GPIO_PULL(config)      (((config) >> 15) & 0x3)


/** 
  Drive strength extraction macro.
    
  Extraction macro used to retrieve the drive strength from a UTLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- UTLMM_GPIO_CFG format configuration.
    
  @sa
  UTLMM_GPIO_CFG 
*/ 
#define UTLMM_GPIO_DRIVE(config)     (((config) >> 17) & 0xF)



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
  @param outval [in] -- uTlmmGpioValueType value (HIGH or LOW).
    
  @sa
  uTlmmGpioValueType, UTLMM_GPIO_CFG
*/
#define UTLMM_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval) \
          (UTLMM_GPIO_CFG((gpio), (func), (dir), (pull), (drive))\
          | (((outval)|0x2) << 0x15))

/** 
  Extraction macro to retrieve the output value of a GPIO.
    
  This macro returns the output value of a particular GPIO configuration when 
  either internally set or set by UTLMM_GPIO_CFG_OUT. This macro is not associated 
  with any value written to the hardware. Its sole purpose is to extract an 
  output value from a configuration. It is typically used when modifying an 
  inactive GPIO configuration. 
    
  @param config [in] -- UTLMM_GPIO_CFG format configuration,
   
  @sa
  UTLMM_GPIO_CFG_OUT 
*/
#define UTLMM_GPIO_GET_OUTVAL(config) (((config) >> 0x15) & 0x1)


/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/


/** 
  State of a GPIO.
    
  The uTlmmGpioStatusType enumerations are used to tell if a particular GPIO is 
  configured to an active or inactive state. 
*/

typedef enum
{
  UTLMM_GPIO_INACTIVE = 0,  /**< -- Inactive state. */
  UTLMM_GPIO_ACTIVE   = 1,  /**< -- Active state. */
/** @cond */
  UTLMM_PLACEHOLDER_TLMMGpioStatusType = 0x7fffffff
/** @endcond */
}uTlmmGpioStatusType;


/*==========================================================================

                      PUBLIC GPIO CONFIGURATION MACROS

==========================================================================*/

/** 
  @addtogroup config_enums 
  @{ 
*/

/** 
  Direction enumerations for configuration. 
    
  The uTlmmGpioDirectionType enumeration specifies the direction to use in the 
  configuration of a GPIO: an INPUT to the MSM or an OUTPUT from the MSM. 
    
  @sa
  UTLMM_GPIO_CFG
*/

typedef enum
{
  UTLMM_GPIO_INPUT  = 0,     /**< -- Set to INPUT. */
  UTLMM_GPIO_OUTPUT = 1,     /**< -- Set to OUTPUT. */

/** @cond */
  UTLMM_PLACEHOLDER_TLMMGpioDirectionType = 0x7fffffff
/**@endcond */

}uTlmmGpioDirectionType;



/** 
  Pull value for a GPIO configuration.
    
  The uTlmmGpioPullType specifies what type of pull (if any) to use in the 
  configuration of a particular GPIO. 
    
  @sa
  UTLMM_GPIO_CFG 
*/

typedef enum
{
  UTLMM_GPIO_NO_PULL    = 0,   /**< -- Do not specify a pull. */
  UTLMM_GPIO_PULL_DOWN  = 0x1, /**< -- Pull the GPIO down. */
  UTLMM_GPIO_KEEPER     = 0x2, /**< -- Designate as a Keeper. */
  UTLMM_GPIO_PULL_UP    = 0x3, /**< -- Pull the GPIO up. */

/** @cond */
  UTLMM_PLACEHOLDER_TLMMGpioPullType = 0x7fffffff
/** @endcond */

}uTlmmGpioPullType;


/** 
  Drive strength values 
    
  The uTlmmGpioDriveType enumeration specifies the drive strength to use in the 
  configuration of a GPIO. 
    
  @sa
  UTLMM_GPIO_CFG 
*/

typedef enum
{
  UTLMM_GPIO_2MA     = 0,    /**< -- Specify a 2 mA drive. */
  UTLMM_GPIO_4MA     = 0x1,  /**< -- Specify a 4 mA drive. */
  UTLMM_GPIO_6MA     = 0x2,  /**< -- Specify a 6 mA drive. */
  UTLMM_GPIO_8MA     = 0x3,  /**< -- Specify an 8 mA drive. */
  UTLMM_GPIO_10MA    = 0x4,  /**< -- Specify a 10 mA drive. */
  UTLMM_GPIO_12MA    = 0x5,  /**< -- Specify a 12 mA drive. */
  UTLMM_GPIO_14MA    = 0x6,  /**< -- Specify a 14 mA drive. */
  UTLMM_GPIO_16MA    = 0x7,  /**< -- Specify a 16 mA drive. */

/** @cond */
  UTLMM_PLACEHOLDER_TLMMGpioDriveType = 0x7fffffff
/** @endcond */

}uTlmmGpioDriveType;


/**
  Output value specification for general purpose I/O.
    
  The uTlmmGpioValueType specifies the value to write to GPIO lines configured as 
  OUTPUT with function select 0 (also known as <i>bit banging</i>).
    
  @sa
  UTLMM_GPIO_CFG_OUT, uTlmm_GpioOut, uTlmm_GpioOutGroup
*/

typedef enum
{
  UTLMM_GPIO_LOW_VALUE,     /**< -- Drive the output LOW. */
  UTLMM_GPIO_HIGH_VALUE,    /**< -- Drive the output HIGH. */

/** @cond */
  UTLMM_PLACEHOLDER_uTlmmGpioValueType = 0x7fffffff
/** @endcond */

}uTlmmGpioValueType;


/** 
  Signal Type alias.
    
  The uTlmmGpioSignalType is an alias for a 32-bit configuration value. The type 
  name can be used by any calling drivers, but the underlying configuration is 
  used as an unsigned 32-bit integer.
    
  @sa
  UTLMM_GPIO_CFG 
*/


/** @cond */
#define UTLMM_PLACEHOLDER_uTlmmGpioSignalType 0x7fffffff
/** @endcond */

typedef uint32 uTlmmGpioSignalType;


/** 
  Enable or disable a GPIO.
    
  The uTlmmGpioEnableType enumerations specify whether a GPIO is to be configured 
  to an active (enable) or inactive (disable) state. 
    
  @sa
  uTlmm_ConfigGpio, uTlmm_ConfigGpioGroup 
*/

typedef enum 
{
  UTLMM_GPIO_DISABLE,  /**< -- Use the internal inactive configuration. */
  UTLMM_GPIO_ENABLE,   /**< -- Use the configuration passed as parameter. */

/** @cond */
  UTLMM_PLACEHOLDER_TLMMGpioEnableType = 0x7fffffff
/** @endcond */

}uTlmmGpioEnableType;


/*==========================================================================

                     APPLICATION PROGRAMMER'S INTERFACE

==========================================================================*/


/* ============================================================================
**  Function : uTlmm_ConfigGpio
** ============================================================================*/
/**
  Configures a GPIO.
 
  This function configures a general purpose I/O or a hardware function 
  multiplexed on a particular GPIO pin. The configuration is based in part on 
  the value for the enable parameter.
  - If this parameter is set to UTLMM_TLMM_GPIO_ENABLE, the configuration 
    associated with the gpio_config parameter is programmed to hardware.
  - If UTLMM_TLMM_GPIO_DISABLE is specified, the GPIO number is extracted from 
    the gpio_config parameter and used to index an internal table to retrieve 
    the inactive (or low-power) configuration associated with the particular GPIO.
 
  @param gpio_config [in] -- Configuration to use whenn programming a GPIO.
  @param enable      [in] -- UTLMM_TLMM_GPIO_ENABLE or UTLMM_TLMM_GPIO_DISABLE.
    
  @return
  TRUE -- GPIO was successfully configured as requested.\n
  FALSE -- Either an invalid GPIO number in the configuration or ownership 
  cannot be obtained from a secure root.
    
  @dependencies
  None.

  @sideeffects
  Ownership for the specified GPIO can be changed, if allowed 
  and required. This may be necessary to provide visibility to 
  generic I/O on certain multiprocessor systems. 
    
  @sa
  UTLMM_GPIO_CFG, uTlmmGpioSignalType
*/ 

boolean
uTlmm_ConfigGpio
(
  uTlmmGpioSignalType uGpioConfig,
  uTlmmGpioEnableType enable
);


/* ============================================================================
**  Function : uTlmm_GetGpioNumber
** ============================================================================*/
/**
  Retrieves a GPIO number.
 
  Retrieves the GPIO number from a specified configuration. This function does 
  the same thing as the macro UTLMM_GPIO_NUMBER.
 
  @param gpio_config   [in] -- Configuration from which to get the GPIO number.
  @param *gpio_number [out] -- GPIO number to return to the caller.
    
  @return
  Returns TRUE even if the GPIO number is invalid for the the current 
  target.
      
  @dependencies
  None.

  @sa
  UTLMM_GPIO_NUMBER
*/

boolean uTlmm_GetGpioNumber
(
  uTlmmGpioSignalType gpio_config, 
  uint32            *gpio_number
);


/* ============================================================================
**  Function : uTlmm_GpioIn
** ============================================================================*/
/**
  Reads a GPIO value.
 
  Reads the value of a GPIO that has been configured as an INPUT.
 
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param *value     [out] -- Value to be returned to the caller.
    
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid number.
    
  @dependencies
  None.
*/

boolean uTlmm_GpioIn
(
  uTlmmGpioSignalType gpio_config, 
  uTlmmGpioValueType*value
);


/* ============================================================================
**  Function : uTlmm_GpioOut
** ============================================================================*/
/**
  Writes a GPIO value.
 
  Writes a value (high or low) to a GPIO that is configured as an OUTPUT, has a 
  function select set to 0, and has no pull. This type of configuration is known 
  as general purpose I/O or bit-banged I/O.
 
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param value       [in] -- Value to write (UTLMM_GPIO_VALUE_HIGH or 
                             UTLMM_GPIO_VALUE_LOW).
    
  @return
  TRUE -- GPIO is valid.\n
  FALSE -- Invalid GPIO.

  @dependencies
  None.
    
  @sa
  uTlmmGpioValueType
*/

boolean uTlmm_GpioOut
(
  uTlmmGpioSignalType gpio_config, 
  uTlmmGpioValueType  value
);


/* ============================================================================
**  Function : uTlmm_GetCurrentConfig
** ============================================================================*/
/**
  Retrieves the current configuration of a GPIO.
 
  This function retrieves the currently programmed configuration for a 
  particular GPIO.
 
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration.
  @param *gpio_config [out] -- Current configuration.
    
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid number.

  @dependencies
  None.
*/

boolean uTlmm_GetCurrentConfig
(
  uint32           gpio_number, 
  uTlmmGpioSignalType *gpio_config
);


/* ============================================================================
**  Function : uTlmm_GetGpioStatus
** ============================================================================*/
/**
  Retrieves the current status of a GPIO.
 
  This function retrieves the current status of a GPIO: whether it is configured 
  to an active state or an inactive state.
 
  @param gpio_number [in] -- GPIO number for which to retrieve the configuration.
  @param *status    [out] -- State of the GPIO: either UTLMM_GPIO_ACTIVE or
                             UTLMM_GPIO_INACTIVE.
 
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid number.
    
  @dependencies
  None.
*/

boolean uTlmm_GetGpioStatus
(
  uint32            gpio_number, 
  uTlmmGpioStatusType *status
);


/* ============================================================================
**  Function : uTlmm_SetInactiveConfig
** ============================================================================*/
/**
  Sets the inactive configuration of a GPIO.
 
  This function is used to modify the inactive or low-power configuration of a 
  GPIO. It modifies the configuration that is programmed when 
  UTLMM_TLMM_GPIO_DISABLE is passed in to the configuration functions.

  This function was originally provided for power debugging, but it can be used 
  elsewhere as necessary. It does not change the current configuration of a GPIO.
 
  @param gpio_number  [in] -- GPIO number for which to retrieve the configuration.
  @param gpio_config [out] -- Configuration to use to replace the low-power
                              configuration in the table.
    
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid number.

  @dependencies
  None.
    
  @sa
  uTlmm_GetInactiveConfig
*/

boolean uTlmm_SetInactiveConfig
(
  uint32            gpio_number, 
  uTlmmGpioSignalType gpio_config
);


/* ============================================================================
**  Function : uTlmm_GetInactiveConfig
** ============================================================================*/
/**
  Retrieves the inactive configuration of a GPIO.
 
  This function retrieves the currently set low-power (inactive) configuration 
  for a particular GPIO.
 
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration
  @param *gpio_config [out] -- Currently set low-power configuration.
    
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid number.

  @dependencies
  None.
    
  @sa
  uTlmm_SetInactiveConfig
*/

boolean uTlmm_GetInactiveConfig
(
  uint32               gpio_number, 
  uTlmmGpioSignalType *gpio_config
);


/* ============================================================================
**  Function : uTlmm_GetOutput
** ============================================================================*/
/**
  Retrieves the output value of a GPIO.
 
  This function retrieves the current output value of a GPIO that has been 
  configured as a generic I/O with output enabled.
 
  @param gpio_number [in] -- GPIO number to use to obtain the output value.
  @param *value     [out] -- Current value of the output GPIO (UTLMM_GPIO_VALUE_LOW 
                             or UTLMM_GPIO_VALUE_HIGH).
    
  @return
  TRUE -- GPIO number is valid.\n
  FALSE -- Invalid GPIO number.
    
  @dependencies
  None.
*/

boolean uTlmm_GetOutput
(
  uint32 gpio_number,
  uTlmmGpioValueType* value
);


#endif

