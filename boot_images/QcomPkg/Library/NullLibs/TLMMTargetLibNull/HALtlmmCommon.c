/*
=================================================================================

                  T O P  L E V E L  M O D E  M U X  ( T L M M ) 
              H A R D W A R E   A B S T R A C T I O N   L A Y E R
                               F O R   8 9 6 0 

FILE:         HALtlmmCommon.c

DESCRIPTION:  
              This modules contains common functionality to the application
              processor and the master processor.

=================================================================================

                             Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------------- 
09/30/15   ck      Cleaned up
03/31/14   na      Update for 64-bit compatibility
06/08/12   dcf     Port for MSM8974. 
08/05/10   dcf     Created.

=================================================================================
     Copyright (c) 2010, 2012, 2014-2015 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                  Qualcomm Technologies Proprietary/GTDR
=================================================================================
*/

/*===============================================================================
                         INCLUDE FILES FOR MODULE
===============================================================================*/
#include <HALcomdef.h>
#include <HALhwio.h>
#include "HALtlmm.h"
#include "HALtlmmTarget.h"


/*===============================================================================
                    GLOBAL STRUCTURES FOR THIS MODULE
===============================================================================*/


/*===============================================================================
  ===============================================================================
                     FUNCTIONS CONTAINED WITHIN THIS MODULE
===============================================================================*/



/*===============================================================================

FUNCTION  
  void HAL_tlmm_WriteConfig( uint16 nGpioNumber, uint16 nConfig )

DESCRIPTION
   Programs the GPIOn_PAGE and GPIOn_CFG registers based on the input 
   parameters.  For APPS processor, this function is stubbed out.

PARAMETERS
   nGpioNumber  - GPIO number to configure
   nGroup       - The registe group that this GPIO number belongs to.
   nConfig      - The masked configuration for this GPIO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/
void HAL_tlmm_WriteConfig( UINTN nGpioNumber, uint32 nConfig )
{
}

/*===============================================================================

FUNCTION  
  void HAL_tlmm_Init ( char ** ppszVersion )

DESCRIPTION
   Does the initializing of the hardware buffers and address structures.

PARAMETERS
   Pointer to the version of this HAL module.

DEPENDENCIES
   None.

RETURN VALUE
   The current version via input parameter.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_Init( char ** ppszVersion )
{
}


/*===============================================================================

FUNCTION  
   void HAL_tlmm_Reset( void )

DESCRIPTION
   Will program the reset state of the TLMM block.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_Reset( void )
{
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_ConfigGpio ( uint32 nWhichConfig )

DESCRIPTION
   Configures the specified GPIO to the requested configuration at the
   provided index.

PARAMETERS
   Packed configuration data containing GPIO number and configuration info.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/

void HAL_tlmm_ConfigGpio( uint32 nWhichConfig )
{
}


/*===============================================================================
FUNCTION  
  void HAL_tlmm_Save ( void )

DESCRIPTION
   Saves the contents of the GPIO registers.

PARAMETERS
   None.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_Save( void )
{
}


/*===============================================================================
FUNCTION  
  void HAL_tlmm_Restore( void )

DESCRIPTION
   Restores the state of the GPIO_OUT and GPIO_OE registers.

PARAMETERS
   None.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/
void HAL_tlmm_Restore( void )
{
}


/*===============================================================================

FUNCTION  
  HAL_tlmm_ReadGpio( uint32 nWhichConfig )

DESCRIPTION
   Returns the boolean value read at the register location extracted 
   from the configuration.

PARAMETERS
   The packed configuration data containing GPIO number and configuration.

DEPENDENCIES
   None. 

RETURN VALUE
   Returns the value "HIGH" or "LOW" as a boolean value.

SIDE EFFECTS
   None.

===============================================================================*/
boolean HAL_tlmm_ReadGpio( uint32 nWhichConfig )
{
  return(FALSE);
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_WriteGpio( uint32 nWhichConfig, bool bValue )

DESCRIPTION
   Outputs the supplied value to the GPIO register at the GPIO location..

DEPENDENCIES
   The GPIO should be configured to be output before calling this function;
   otherwise, the value written to the GPIO may not take effect.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_WriteGpio( uint32 nWhichConfig, boolean bValue )
{
}


/*===============================================================================

FUNCTION  
   void HAL_tlmm_WriteGpioGroup( uint32 nWhichConfigSet[], uint16 nWhatSize )

DESCRIPTION
   Outputs a given value to a group of GPIOs based on the input parameters.

PARAMETERS
   nWhichConfigSet[]  - The array of GPIO configurations to write to.
   nWhatSize          - The size of the above array.
   bWriteVal          - Boolean representation of the value to write:
                            TRUE  = HIGH 
                            FALSE = LOW

DEPENDENCIES
   The GPIO should be configured to be output before calling this function;
   otherwise, the value written to the GPIO may not take effect.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_WriteGpioGroup( const uint32 nWhichConfigSet[], 
                              uint16 nWhatSize,
                              boolean bWriteVal )
{
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_ConfigGpioGroup ( uint32 nWhichGpioSet[], 
                                  uint32 nWhichCfgSet[], 
                                  uint8 size )

DESCRIPTION
   Configures a group of GPIOs based on the supplied input parameters.  Each
   array contains a GPIO/Configuration set, but we extract only the GPIO from
   the first array and the configuration from the second.  This allows GPIO
   groups to be set to several different configurations, such as primary,
   sleep, or default group configuration.

PARAMETERS
   nWhichGpioSet[]  - An array of GPIO configurations to extract the GPIO 
                      numbers to configure.
   nWhichCfgSet[]   - An array of GPIO configurations to extract the GPIO
                      configuration information to set the above GPIOs to.
   nWhatSize        - The size of the first array.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void HAL_tlmm_ConfigGpioGroup( const uint32 nWhichGpioSet[],
                               uint16 nWhatSize )
{
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_TristateGpioGroup ( uint32 nWhichGpioSet[], 
                                  uint32 nWhichCfgSet[], 
                                  uint8 size )

DESCRIPTION
   Tristates a group of GPIOs to the given input set of gpio configurations.  

PARAMETERS
   nWhichGpioSet[]  - An array of GPIO configurations to extract the GPIO 
                      numbers to configure.
   nWhichCfgSet[]   - An array of GPIO configurations to extract the GPIO
                      configuration information to set the above GPIOs to.
   nWhatSize        - The size of the first array.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/

void HAL_tlmm_TristateGpioGroup( const uint32 nWhichGpioSet[],
                               uint16 nWhatSize )
{
}

/*===============================================================================

FUNCTION  
  void HAL_tlmm_TristateGpio ( uint32 nWhichConfig )

DESCRIPTION
   Tristate the specified GPIO to the requested configuration at the
   provided index.

PARAMETERS
   Packed configuration data containing GPIO number and configuration info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===============================================================================*/
void HAL_tlmm_TristateGpio( uint32 nWhichConfig )
{
}


/*===============================================================================

FUNCTION  
  void HAL_tlmm_GetOutput ( uint32 nWhichGpio, boolean value )

DESCRIPTION
   Reads the output state of a GPIO configured as a generic bit-banged
   operation.

PARAMETERS
   GPIO number to read output from.

DEPENDENCIES
  GPIO must be programmed as a generic output, or this will return FALSE.

RETURN VALUE
  TRUE  - Corresponds to a HIGH value.
  FALSE - Corresponds to a LOW value.

SIDE EFFECTS
  None.
===============================================================================*/
boolean HAL_tlmm_GetOutput( uint32 nWhichGpio )
{
    return(FALSE);
}

/*===============================================================================

FUNCTION  
  void HAL_tlmm_UpdateConfig ( uint32 nWhichGpio )

DESCRIPTION
   Updates only the configuration register for a particular GPIO.  Direction
   programming is not done in this function.

PARAMETERS
   Configuration to program.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/
void HAL_tlmm_UpdateConfig( uint32 nWhichConfig )
{
}

/*===============================================================================

FUNCTION  
  void HAL_tlmm_UpdateDirection ( uint32 nWhichGpio )

DESCRIPTION
   Updates only the OE register for a particular GPIO.  Configuration and 
   function select are not programmed by this function.

PARAMETERS
   Configuration to program direction for.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===============================================================================*/
void HAL_tlmm_UpdateDirection( uint32 nWhichConfig )
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetPort(HAL_tlmm_PortType ePort, uint32 mask, uint32 value)

DESCRIPTION
   Selects the specified port if it is defined.

PARAMETERS
   ePort - The requested port to configure.
   mask  - The mask to use for this port.
   value - The value to be written to the port.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetPort(HAL_tlmm_PortType ePort, uint32 mask, uint32 value)
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_GetNumberGpio( uint32* pnNumGpio )

DESCRIPTION
   Returns the number of GPIO supported on this target.

PARAMETERS
   pnNumGpio - Pointer to the number of GPIO to return.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_GetNumberGpio( uint32* pnNumGPIO )
{
  *pnNumGPIO = HAL_TLMM_NUM_GPIO;
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetPull( uint32 nGpioNumber, HAL_tlmm_PullType nPull )

DESCRIPTION
   Updates the PULL value of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   ePull       - The HAL_tlmm_PullType value to set to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetPull( uint32 nGpioNumber, HAL_tlmm_PullType ePull )
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetDirection( uint32 nGpioNumber, HAL_tlmm_DirType nPull )

DESCRIPTION
   Changes the direction of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   eDirection  - The HAL_tlmm_DirType value to change to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetDirection( uint32 nGpioNumber, HAL_tlmm_DirType eDirection )
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetDriveStrength( uint32 nGpioNumber, HAL_tlmm_DriveType nPull )

DESCRIPTION
   Changes the drive strength of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   eDrive      - The HAL_tlmm_DriveType value to change to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetDriveStrength( uint32 nGpioNumber, HAL_tlmm_DriveType eDrive )
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_SetFunction( uint32 nGpioNumber, HAL_tlmm_DriveType nPull )

DESCRIPTION
   Changes the function select of a GPIO.  Only use this API if you know
   what you're doing as HW may enter a bad state if not properly set.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   nFuncVal    - The Function select to set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_SetFunction( uint32 nGpioNumber, uint32 nFuncVal )
{
}


/*=========================================================================

FUNCTION  
  void HAL_tlmm_GetConfig( uint32* pnNumGpio )

DESCRIPTION
   Returns the current configuration of a GPIO read from HW.

PARAMETERS
   nGpioNumber - The GPIO number to read the configuration for.
   tGpio       - The data structure to hold the configuration information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void HAL_tlmm_GetConfig( uint32 nGpioNumber, HAL_tlmm_GpioType* tGpio )
{
}

/*=============================================================================*/

