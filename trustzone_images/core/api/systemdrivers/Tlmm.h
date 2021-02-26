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

$Header: //components/rel/core.tz/2.0.2/api/systemdrivers/Tlmm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/14   dcf     Initial version for TZ.

=============================================================================
Copyright (c) 2014-2016 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary and Confidential. 
===========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "com_dtypes.h"


/*==========================================================================

                   PUBLIC DEFINITIONS FOR MODULE

==========================================================================*/

/* 
  Main configuration macro. 
    
  This main configuration macro is used when configuring a GPIO. It 
  utilizes the TLMM_GPIO_VERSION definition to ensure that configurations conform 
  to the wider field size.\n
  
  @param gpio  [in] -- GPIO number to program.
  @param func  [in] -- Function selection (0 for generic I/O).
  @param dir   [in] -- Direction (input or output).
  @param pull  [in] -- Pull value (up, down, or none).
  @param drive [in] -- Drive strength to program.
   
  @sa
  Tlmm_ConfigGpio
*/

#define TLMM_GPIO_CFG(gpio, func, dir, pull, drive) \
         (((gpio) & 0x3FF)<< 4 | \
          ((func) & 0xF)|        \
          ((dir)  & 0x1) << 14|  \
          ((pull) & 0x3) << 15|  \
          ((drive)& 0xF) << 17)

/**
  GPIO number extraction macro.
    
  This extraction macro is used to retrieve the GPIO number from a TLMM_GPIO_CFG
  formatted configuration.
 
  @param config [in] -- TLMM_GPIO_CFG format configuration.
 
  @sa
  TLMM_GPIO_CFG
*/
#define TLMM_GPIO_NUMBER(config)    (((config) >> 4) & 0x3FF)

/** 
  Function select extraction macro. 
    
  Extraction macro used to retrieve the function select from a TLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- TLMM_GPIO_CFG format configuration.
    
  @sa
  TLMM_GPIO_CFG
*/
#define TLMM_GPIO_FUNCTION(config)   ((config) & 0xF)

/** 
  Direction value extraction macro.
    
  Extraction macro used to retrieve the direction from a TLMM_GPIO_CFG formatted 
  configuration. 
    
  @param config [in] -- TLMM_GPIO_CFG format configuration.
    
  @sa
  TLMM_GPIO_CFG 
*/ 
#define TLMM_GPIO_DIRECTION(config) (((config) >> 14) & 0x1)

/** 
  Pull value extraction macro.
    
  This extraction macro is used to retrieve the pull value from a TLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- TLMM_GPIO_CFG format configuration.
    
  @sa
  TLMM_GPIO_CFG
*/
#define TLMM_GPIO_PULL(config)      (((config) >> 15) & 0x3)

/** 
  Drive strength extraction macro.
    
  Extraction macro used to retrieve the drive strength from a TLMM_GPIO_CFG 
  formatted configuration. 
    
  @param config [in] -- TLMM_GPIO_CFG format configuration.
    
  @sa
  TLMM_GPIO_CFG 
*/ 
#define TLMM_GPIO_DRIVE(config)     (((config) >> 17) & 0xF)


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
  @param outval [in] -- TLMMGpioValueType value (HIGH or LOW).
    
  @sa
  TLMMGpioValueType, TLMM_GPIO_CFG
*/
#define TLMM_GPIO_CFG_OUT(gpio, func, dir, pull, drive, outval) \
          (TLMM_GPIO_CFG((gpio), (func), (dir), (pull), (drive))\
          | (((outval)|0x2) << 0x15))


/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/



/** 
  @addtogroup dir_enums 
  @{ 
*/ 

/** 
  Mode of a GPIO.
    
  Enumeration allowing a particular GPIO ID to be toggled 
  between its primary function and an I/O mode. 
*/

typedef enum
{
   TLMM_GPIO_MODE_PRIMARY,
   TLMM_GPIO_MODE_IO,
   TLMM_PLACEHOLDER_TLMMGpioModeType = 0x7fffffff
}TLMMGpioModeType;



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
    
  The TLMMGpioDirectionType enumeration specifies the direction to use in the 
  configuration of a GPIO: an INPUT to the MSM or an OUTPUT from the MSM. 
    
  @sa
  TLMM_GPIO_CFG
*/

typedef enum
{
  TLMM_GPIO_INPUT  = 0,     /**< -- Set to INPUT. */
  TLMM_GPIO_OUTPUT = 1,     /**< -- Set to OUTPUT. */

/** @cond */
  TLMM_PLACEHOLDER_TLMMGpioDirectionType = 0x7fffffff
/**@endcond */

}TLMMGpioDirectionType;



/** 
  Pull value for a GPIO configuration.
    
  The TLMMGpioPullType specifies what type of pull (if any) to use in the 
  configuration of a particular GPIO. 
    
  @sa
  TLMM_GPIO_CFG 
*/

typedef enum
{
  TLMM_GPIO_NO_PULL    = 0,   /**< -- Do not specify a pull. */
  TLMM_GPIO_PULL_DOWN  = 0x1, /**< -- Pull the GPIO down. */
  TLMM_GPIO_KEEPER     = 0x2, /**< -- Designate as a Keeper. */
  TLMM_GPIO_PULL_UP    = 0x3, /**< -- Pull the GPIO up. */

/** @cond */
  TLMM_PLACEHOLDER_TLMMGpioPullType = 0x7fffffff
/** @endcond */

}TLMMGpioPullType;


/** 
  Drive strength values 
    
  The TLMMGpioDriveType enumeration specifies the drive strength to use in the 
  configuration of a GPIO. 
    
  @sa
  TLMM_GPIO_CFG 
*/

typedef enum
{
  TLMM_GPIO_2MA     = 0,    /**< -- Specify a 2 mA drive. */
  TLMM_GPIO_4MA     = 0x1,  /**< -- Specify a 4 mA drive. */
  TLMM_GPIO_6MA     = 0x2,  /**< -- Specify a 6 mA drive. */
  TLMM_GPIO_8MA     = 0x3,  /**< -- Specify an 8 mA drive. */
  TLMM_GPIO_10MA    = 0x4,  /**< -- Specify a 10 mA drive. */
  TLMM_GPIO_12MA    = 0x5,  /**< -- Specify a 12 mA drive. */
  TLMM_GPIO_14MA    = 0x6,  /**< -- Specify a 14 mA drive. */
  TLMM_GPIO_16MA    = 0x7,  /**< -- Specify a 16 mA drive. */

/** @cond */
  TLMM_PLACEHOLDER_TLMMGpioDriveType = 0x7fffffff
/** @endcond */

}TLMMGpioDriveType;


/**
  Output value specification for general purpose I/O.
    
  The TLMMGpioValueType specifies the value to write to GPIO lines configured as 
  OUTPUT with function select 0 (also known as <i>bit banging</i>).
    
  @sa
  TLMM_GPIO_CFG_OUT, Tlmm_GpioOut, Tlmm_GpioOutGroup
*/

typedef enum
{
  TLMM_GPIO_LOW_VALUE,     /**< -- Drive the output LOW. */
  TLMM_GPIO_HIGH_VALUE,    /**< -- Drive the output HIGH. */

/** @cond */
  TLMM_PLACEHOLDER_TLMMGpioValueType = 0x7fffffff
/** @endcond */

}TLMMGpioValueType;


/** 
  GPIO Identity Key type.
    
  The TLMMGpioIdKeyType identifies a particular GPIO configuration 
  pertaining to a GPIO string name. 
    
  @sa
  Tlmm_GetGpioId, Tlmm_ConfigGpioId, 
  Tlmm_ConfigGpioIdInactive 
*/
typedef uint32 TLMMGpioIdKeyType;


/** 
  Enable or disable a GPIO.
    
  The TLMMGpioEnableType enumerations specify whether a GPIO is to be configured 
  to an active (enable) or inactive (disable) state. 
    
  @sa
  Tlmm_ConfigGpio, Tlmm_ConfigGpioGroup 
*/

typedef enum 
{
  TLMM_GPIO_DISABLE,  /**< -- Use the internal inactive configuration. */
  TLMM_GPIO_ENABLE,   /**< -- Use the configuration passed as parameter. */

/** @cond */
  TLMM_PLACEHOLDER_TLMMGpioEnableType = 0x7fffffff
/** @endcond */

}TLMMGpioEnableType;

/** 
  @}
*/ /* end_group config_enums */


/** 
  @addtogroup block_configs 
  @{ 
*/


/** 
  TLMM GPIO Config ID Type.
    
  This type defines the configurable portions of a GPIO whose 
  pin number, and function select are derived at runtime through
  device config using the API Tlmm_GetGpioId(). 
   
  @sa
  Tlmm_GetGpioId, Tlmm_ConfigGpioId
*/

typedef struct
{
  TLMMGpioDirectionType eDirection;
  TLMMGpioPullType ePull;
  TLMMGpioDriveType eDriveStrength;

}TLMMGpioConfigIdType;


/**
  TLMM GPIO settings type

  This structure defines the parameters which can be read from the 
  TLMM HW for testing and other purposes.
*/

typedef struct
{
  uint32                   nGpioNumber;
  uint32                   nFunctionSelect;
  TLMMGpioConfigIdType Settings;
  TLMMGpioValueType         eOutputDrive;
}TLMMGpioIdSettingsType;

/** 
  @}
*/ /* end_group block_configs */



/* ============================================================================
**  Function : Tlmm_GetGpioId
** ============================================================================*/
/**
  Retrieves a GPIO identifier corresponding to an input string.
 
  This API retrieves a GPIO ID corresponding to a GPIO string name.  The string
  name must match exactly the expected name for the HW functionality.
 
  @param *pszGpio[in] -- The name of the GPIO string to search for.
  @param *pnGpioId[out] -- The identification pertaining to the above string.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
 
  @sa
  Tlmm_ConfigGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_GetGpioId(const char* pszGpio, TLMMGpioIdKeyType* pnGpioId);


/* ============================================================================
**  Function : Tlmm_ConfigGpioId
** ============================================================================*/
/**
  Configures a GPIO based on identifier and user settings.
 
  This API configures a GPIO based on a GPIO Identifier and user configuration
  settings.
 
  @param *nGpioId[in] -- The GPIO Identifier to configure.
  @param *pUserSettings[in] -- The user settings to apply to the GPIO for the
                               identifier in parameter 2.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_ConfigGpioId(TLMMGpioIdKeyType nGpioId, TLMMGpioConfigIdType* pUserSettings);


/* ============================================================================
**  Function : Tlmm_ConfigGpioIdInactive
** ============================================================================*/
/**
  Configures a GPIO to a predefined low-power state.
 
  This API configures a GPIO based on a GPIO Identifier to a predetermined
  low-power configuration defined for a particular platform.
 
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_ConfigGpioIdInactive(TLMMGpioIdKeyType nGpioId);


/* ============================================================================
**  Function : Tlmm_GpioIdOut
** ============================================================================*/
/**
  Outputs the state of a GPIO configure for output.
 
  This API configures a GPIO based on a GPIO Identifier to a predetermined
  low-power configuration defined for a particular platform.
 
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_GpioIdOut(TLMMGpioIdKeyType nGpioId, TLMMGpioValueType eValue);


/* ============================================================================
**  Function : Tlmm_GpioIdIn
** ============================================================================*/
/**
  Reads the state of the GPIO
 
  This API reads the input value of a GPIO signal.
 
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_GpioIdIn(TLMMGpioIdKeyType nGpioId, TLMMGpioValueType *eValue);


/* ============================================================================
**  Function : Tlmm_ReleaseGpioId
** ============================================================================*/
/**
  Releases an auto-lock on a GPIO.
 
  This API releases a lock which is automatically applied when the GPIO ID is
  retrieved.  The lock can only be released by the owner of GPIO ID.
 
  @param *nGpioId[in] -- The GPIO Identifier to release the lock for.
    
  @return
  DAL_SUCCESS -- The lock was released.
  DAL_ERROR -- The GPIO ID is not allowed to unlock this GPIO.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_ReleaseGpioId(TLMMGpioIdKeyType nGpioId); 


/* ============================================================================
**  Function : Tlmm_SelectGpioIdMode
** ============================================================================*/
/**
  Allows the owner of a GPIO ID to toggle between IO mode and functional mode.
 
  This is an optional API to allow a GPIO owner who uses the GPIO ID to switch between
  the functional mode (function select for a particular platform) and IO mode.
 
  @param *nGpioId[in] -- The GPIO Identifier to operate on.
  @param eMode[in] -- Specified whether to use IO mode or functional
                      mode.  TRUE specifies to use IO mode, FALSE specifies
                      to use functional mode.  Note, in cases where the
                      functional mode is also the IO mode, no change
                      will take place.
  @param pUserSettings -- Optional user settings to apply when switching mode.  A
                          null value will keep the current settings.
    
  @return
  DAL_SUCCESS -- The specified operation completed successfully.
  DAL_ERROR -- The GPIO ID does not have permission to change the functional
               mode of this GPIO.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_SelectGpioIdMode(TLMMGpioIdKeyType nGpioId, TLMMGpioModeType eMode, TLMMGpioConfigIdType* pUserSettings); 


/* ============================================================================
**  Function : Tlmm_GetGpioIdConfig
** ============================================================================*/
/**
  Reads the hardware and returns the curreng GPIO configuration.
 
  This API releases a lock which is automatically applied when the GPIO ID is
  retrieved.  The lock can only be released by the owner of GPIO ID.
 
  @param *nGpioId[in] -- The GPIO Identifier to retrieve the configuration for.
  @param *pGpioSettings[inout] -- The settings container to store the information in.
    
  @return
  DAL_SUCCESS -- The configuration was retrieved.
  DAL_ERROR -- The GPIO ID is not invalid.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/

DALResult Tlmm_GetGpioIdSettings(TLMMGpioIdKeyType nGpioId, TLMMGpioIdSettingsType* pGpioSettings); 


/* ============================================================================
**  Function : Tlmm_Init
** ============================================================================*/
/**
  API to initialize the TLMM driver.
 
  This API will do everything necessary to initialize the TLMM driver so that calls
  can be made to configure GPIO as needed.
    
  @return
  E_SUCCESS -- The driver successfully completed initialization.
  E_FAILURE -- The driver could not initialize.
    
  @dependencies
  None.
  
  @sa
  Tlmm_GetGpioId, Tlmm_GpioConfigIdType
*/
int Tlmm_Init(void);

#endif

