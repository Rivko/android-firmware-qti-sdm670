/** 
  @file  EFITlmm.h
  @brief Top-Level Mode Multiplexer Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/03/12   vishalo Merge in Techpubs Doxygen change
 03/24/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/26/12   vishalo Merge in Techpubs Doxygen change
 11/02/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/13/11   dcf     Created.

=============================================================================*/
#ifndef __EFITLMM_H__
#define __EFITLMM_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_tlmm_constants
  Protocol version.
*/
#define EFI_TLMM_PROTOCOL_REVISION 0x0000000000000001

/*  Protocol GUID definition */
/** @ingroup efi_tlmm_protocol */
#define EFI_TLMM_PROTOCOL_GUID \
   { 0xad9aec18, 0x7bf0, 0x4809, {0x9e, 0x96, 0x30, 0x12, 0x30, 0x9f, 0x3d, 0xf7 } }



/* Real-time (RT) Protocol ID */
/** @ingroup efi_tlmm_protocol */
#define EFI_TLMM_RT_PROTOCOL_GUID \
  { 0xCC3F4DA6, 0xA51E, 0x4FB7, { 0x98, 0xD1, 0xA0, 0x6E, 0xB8, 0x5D, 0x8E, 0x1E } }

/** @ingroup efi_tlmm_constants
  Real-time protocol version.
*/
#define EFI_TLMM_RT_PROTOCOL_VERSION 0x00010000


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/**
  External reference to the DALTLMM Protocol GUID.
*/
extern EFI_GUID gEfiTLMMProtocolGuid;

extern EFI_GUID gEfiTLMMRTProtocolGuid;
/** @endcond */

/** @addtogroup efi_tlmm_constants 
@{ */
/** Used to ensure a 32-bit enumeration size. */
#define GPIO_ENUM_FORCE32(name) GPIO_##name##_SIZE = 0x7FFFFFFF

/** 
  @par Summary
  Main configuration macro. 

  @param[in]  gpio   GPIO number to program.
  @param[in]  func   Function selection (0 for generic I/O).
  @param[in]  dir    Direction (input or output); see #TLMM_DirectionType 
                     for details.
  @param[in]  pull   Pull value (up, down, or none); see #TLMM_PullType 
                     for details.
  @param[in]  drive  Drive strength to program; see #TLMM_DriveStrengthType 
                     for details. 

  @par Description
  This main configuration macro is used when configuring a GPIO. It 
  utilizes the GPIO_ENUM_FORCE32 definition to ensure that configurations 
  conform to the wider field size. 
  @par
  This macro creates a GPIO configuration that is passed into functions 
  requiring a uint32 or UINT32 configuration parameter. 
*/
/* This is a duplicate of the DAL_GPIO_CFG structure from DDITlmm.h used 
  on DAL environments.  */
#define EFI_GPIO_CFG(gpio, func, dir, pull, drive) \
         (((gpio) & 0x3FF)<< 4 | \
          ((func) & 0xF)|        \
          ((dir)  & 0x1) << 14|  \
          ((pull) & 0x3) << 15|  \
          ((drive)& 0xF) << 17| 0x20000000)
/** @} */ /* end_addtogroup efi_tlmm_constants */


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** @cond */
/**
  Protocol declaration.
 */
typedef struct _EFI_TLMM_PROTOCOL EFI_TLMM_PROTOCOL;
/** @endcond */

/** @addtogroup efi_tlmm_data_types 
@{ */
/** 
  Enable/disable specification for a GPIO. This enumeration specifies to the 
  TLMM driver whether a GPIO configuration must be set to an Active state 
  (via the configuration parameter setting) or Inactive state (via the 
  internal low-power table setting). 
   
  @note1hang When TLMM_GPIO_DISABLE is passed in to a configuration 
             function, the GPIO number is extracted from the configuration 
             and used to index the internal low-power table to retrieve the 
             low-power setting. 
*/
typedef enum
{
  TLMM_GPIO_DISABLE = 0,        /**< Set to low power. */ 
  TLMM_GPIO_ENABLE  = 1,        /**< Set to a specified active state. */ 
/** @cond */
  GPIO_ENUM_FORCE32(ENABLE)
/** @endcond */
}TLMM_EnableType;


/** 
  Output high/low specification for a GPIO configured as an output. This 
  enumeration specifies whether a generic output pin must be driven 
  high or low by the EFI_TLMM_GPIO_OUT() function. 
   
*/
typedef enum
{
  GPIO_LOW_VALUE  = 0,          /**< Set output to low. */  
  GPIO_HIGH_VALUE = 1,          /**< Set output to high. */ 
/** @cond */
  GPIO_ENUM_FORCE32(VALUE)
/** @endcond */
}TLMM_ValueType;


/** 
  Direction enumeration for configuration. This enumeration specifies the 
  direction to use in a GPIO's configuration: an input to the MSM device 
  or an output from the MSM device. 
*/
typedef enum
{
  GPIO_INPUT   = 0,             /**< Set to input. */  
  GPIO_OUTPUT  = 1,             /**< Set to output. */ 
/** @cond */
  GPIO_ENUM_FORCE32(DIRECTION)
/** @endcond */
}TLMM_DirectionType;


/** 
  Pull value for a GPIO configuration. This enumeration specifies what type 
  of pull (if any) to use in a specific GPIO's configuration. 
*/
typedef enum
{
  GPIO_NO_PULL   = 0,           /**< None; do not specify a pull.*/      
  GPIO_PULL_DOWN = 1,           /**< Pull the GPIO down.  */   
  GPIO_KEEPER    = 2,           /**< Designate as a keeper. */   
  GPIO_PULL_UP   = 3,           /**< Pull the GPIO up. @newpage */        
/** @cond */
  GPIO_ENUM_FORCE32(PULL)
/** @endcond */
}TLMM_PullType;


/** 
  Drive strength values. This enumeration specifies the drive strength 
  to use in a GPIO's configuration. 
*/
/* see also  DAL_GPIO_CFG */
typedef enum
{                                     
  GPIO_2MA   = 0,               /**< 2 mA drive. */ 
  GPIO_4MA   = 1,               /**< 4 mA drive. */ 
  GPIO_6MA   = 2,               /**< 6 mA drive. */ 
  GPIO_8MA   = 3,               /**< 8 mA drive. */
  GPIO_10MA  = 4,               /**< 10 mA drive. */
  GPIO_12MA  = 5,               /**< 12 mA drive. */
  GPIO_14MA  = 6,               /**< 14 mA drive. */
  GPIO_16MA  = 7,               /**< 16 mA drive. */
/** @cond */
  GPIO_ENUM_FORCE32(DRIVESTR)
/** @endcond */
}TLMM_DriveStrengthType;
/** @} */ /* end_addtogroup efi_tlmm_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_TLMM_CONFIG_GPIO */ 
/** @ingroup efi_tlmm_config_gpio
  @par Summary
  Configures a GPIO or a hardware function multiplexed on a specific GPIO pin. 

  @param[in]  config       Configuration to use when programming a GPIO.
  @param[in]  enable       TLMM_GPIO_ENABLE or TLMM_GPIO_DISABLE; 
                           see #TLMM_EnableType for details.

  @par Description
  The configuration is based in part on the value for the enable parameter.
  - If this parameter is set to TLMM_GPIO_ENABLE, the configuration 
    associated with the config parameter is programmed to hardware.
  - If TLMM_GPIO_DISABLE is specified, the GPIO number is extracted from 
    the config parameter and used to index an internal table to retrieve 
    the inactive (or low-power) configuration associated with the specific 
    GPIO.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/ 
typedef 
EFI_STATUS
(EFIAPI *EFI_TLMM_CONFIG_GPIO)(
   IN UINT32 config,                            
   IN UINT32 enable);


/* EFI_TLMM_CONFIG_GPIO_GROUP */ 
/** @ingroup efi_tlmm_config_gpio_group
  @par Summary
  Configures a group of GPIOs or a hardware function multiplexed on a specific 
  GPIO pin. 

  @param[in] enable        TLMM_GPIO_ENABLE or TLMM_GPIO_DISABLE; 
                           see #TLMM_EnableType for details.
  @param[in] config_group  Configuration array to use when programming a GPIO.
  @param[in] size          Number of GPIO configurations in the config_group 
                           array.
    
  @par Description
  The configuration is based in part on the value for the enable parameter.
  - If this parameter is set to TLMM_GPIO_ENABLE, the configuration 
    associated with the config_group parameter is programmed to hardware.
  - If TLMM_GPIO_DISABLE is specified, the GPIO number is extracted from 
    the config_group parameter and used to index an internal table to retrieve 
    the inactive (or low-power) configuration associated with the specific 
    GPIO.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TLMM_CONFIG_GPIO_GROUP)(
   IN UINT32 enable,
   IN UINT32 *config_group,
   IN UINT32 size);


/* EFI_TLMM_GPIO_IN */ 
/** @ingroup efi_tlmm_gpio_in
  @par Summary
  Reads the value of a GPIO that has been configured as an input.

  @param[in]   config      Configuration from which to get the GPIO number.
  @param[out]  value       Value to be returned to the caller.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TLMM_GPIO_IN)(
   IN UINT32 config,
   OUT UINT32 *value);


/* EFI_TLMM_GPIO_OUT */ 
/** @ingroup efi_tlmm_gpio_out
  @par Summary
  Writes a GPIO value.

  @param[in] config       Configuration from which to get the GPIO number.
  @param[in] value        Value to write (TLMM_GPIO_VALUE_HIGH or
                          TLMM_GPIO_VALUE_LOW); see #TLMM_ValueType for 
                          details.

  @par Description
  This function writes a value (high or low) to a GPIO that has the following 
  conditions:
  - Configured as an output; see #TLMM_DirectionType for details  
  - Function select is set to 0 
  - Pull is disabled (GPIO_NO_PULL); see #TLMM_PullType for details 
  @par
  This type of configuration is known as a general-purpose I/O or 
  bit-banged I/O.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TLMM_GPIO_OUT)(
   IN UINT32 config,
   IN UINT32 value);


/* EFI_TLMM_SET_INACTIVE_CONFIG */ 
/** @ingroup efi_tlmm_set_inactive_config
  @par Summary
  Specifies the inactive configuration of a GPIO.

  @param[in]  gpio_number   GPIO number for which to retrieve the configuration.
  @param[in]  config        Configuration to use to replace the low-power
                            configuration in the internal table.

  @par Description
  This function is used to modify the inactive (or low-power) configuration 
  of a GPIO. It modifies the configuration that is programmed when
  TLMM_GPIO_DISABLE is passed in to the configuration functions.
  @par
  This function was originally provided for power debugging, but it can be used
  elsewhere as necessary in cases where the default low-power setting is not
  adequate. It does not change the current configuration of a GPIO.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
/* see also DalTlmm_GetInactiveConfig */
typedef
EFI_STATUS
(EFIAPI *EFI_TLMM_SET_INACTIVE_CONFIG)(
   IN UINT32 gpio_number,
   IN UINT32 config);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_tlmm_protocol
  @par Summary
  Top-Level Mode Multiplexer (TLMM) Protocol interface.

  @par Parameters
  @inputprotoparams{tlmm_proto_params.tex} 
*/
struct _EFI_TLMM_PROTOCOL {
   UINT64                       Revision;
   EFI_TLMM_CONFIG_GPIO         ConfigGpio;
   EFI_TLMM_CONFIG_GPIO_GROUP   ConfigGpioGroup;
   EFI_TLMM_GPIO_IN             GpioIn;
   EFI_TLMM_GPIO_OUT            GpioOut;
   EFI_TLMM_SET_INACTIVE_CONFIG SetInactiveConfig;
};

#endif /* __EFITLMM_H__ */
