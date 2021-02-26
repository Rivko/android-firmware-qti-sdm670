#ifndef HALTLMMTARGET_H
#define HALTLMMTARGET_H
/*
=================================================================================

                    T O P  L E V E L  M O D E  M U X  (TLMM) 
              H A R D W A R E   A B S T R A C T I O N   L A Y E R

FILE:         HALtlmmTarget.h

DESCRIPTION:  
              Internal definitions for use by this module only.

=================================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/TLMMTargetLibNull/HALtlmmTarget.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------- 
06/08/12   dcf     Port to MSM8974. 
08/05/10   dcf     Ported from 8660.

=================================================================================
             Copyright (c) 2010, 2014-2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  Qualcomm Technologies Proprietary/GTDR
=================================================================================
*/

/*-------------------------------------------------------------------------------
** Types
**-----------------------------------------------------------------------------*/

#define HAL_TLMM_NUM_GPIO          150


/*==============================================================================
  The following are GPIO configuration definitions that will be used to 
  configure the GPIOs.
  ----------------------------------------------------------------------------*/

/* The enumeration for GPIO value(high or low) type. */
typedef enum
{
    HAL_TLMM_LOW_VALUE  = 0,
    HAL_TLMM_HIGH_VALUE = 1
}HAL_tlmm_ValueType;




/* ==============================================================================
   The following macro takes the configuration values from the BSP format used
   by the driver and restructures them to fit the format of the configuration
   registers for this target.
   ----------------------------------------------------------------------------*/

#define HAL_GPIO_CONFIG_MASK(cfg)   \
    ((HAL_PULL_VAL(cfg)   << 0x0) | \
     (HAL_FUNC_VAL(cfg)   << 0x2) | \
     (HAL_DRVSTR_VAL(cfg) << 0x6) | \
     (HAL_DIR_VAL(cfg)    << 0x9) )



/*===============================================================================
  Functions defined in this module for use internally by the TLMM HAL.
  -----------------------------------------------------------------------------*/

void HAL_tlmm_WriteConfig( UINTN nWhichGpio, uint32 nWhichConfig );

/*===============================================================================

FUNCTION  
  void HAL_tlmm_WriteConfig( uint32 nWhichGpio,
                             uint32 nWhichConfig )

DESCRIPTION
   Programs the GPIO configuration register to the specified configuration.

PARAMETERS
   nGpioNumber  - The Gpio number to configure.
   nConfig      - The configuration to set the GPIO to.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/

#endif /* #ifndef HALTLMMTARGET_H */

