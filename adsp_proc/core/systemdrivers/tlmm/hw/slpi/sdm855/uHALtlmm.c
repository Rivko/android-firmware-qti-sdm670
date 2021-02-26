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

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/tlmm/hw/slpi/sdm855/uHALtlmm.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------
08/05/10   dcf     Created.

=================================================================================
             Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=================================================================================
*/

/*===============================================================================
                         INCLUDE FILES FOR MODULE
===============================================================================*/
#include <HALcomdef.h>
#include <HALhwio.h>
#include "uHALtlmm.h"
#include <msmhwiobase.h>
#include "HALtlmmSSCHWIO.h"


/*===============================================================================
                    GLOBAL STRUCTURES FOR THIS MODULE
===============================================================================*/

typedef struct
{
  uint32 nCfgAddr;
  uint32 nInOutAddr;
}uHAL_tlmm_ConfigsType;




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


static uHAL_tlmm_ConfigsType uHAL_tlmm_Configs[] =
{
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_0),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_0)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_1),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_1)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_2),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_2)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_3),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_3)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_4),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_4)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_5),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_5)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_6),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_6)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_7),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_7)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_8),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_8)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_9),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_9)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_10),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_10)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_11),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_11)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_12),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_12)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_13),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_13)
  },
#if 0
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_14),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_14)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_15),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_15)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_16),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_16)
  },
  {
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_CFG_17),
    HWIO_ADDR(SSC_TLMM_LITE_GPIO_IN_OUT_17)
  },
#endif
};

#define HAL_TLMM_TOTAL_GPIO sizeof(uHAL_tlmm_Configs)/sizeof(uHAL_tlmm_ConfigsType)


static uHAL_tlmm_InitType tHALTLMMInit = 
{
  SSC_BASE,
  HAL_TLMM_TOTAL_GPIO
};


/*===============================================================================
  ===============================================================================
                     FUNCTIONS CONTAINED WITHIN THIS MODULE
===============================================================================*/



/*===============================================================================

FUNCTION  
  void uHAL_tlmm_Init ( uHAL_tlmm_InitType* pInit)

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

void uHAL_tlmm_Init( uHAL_tlmm_InitType* pInit )
{
  uint32 nIdx = 0;

  /* Initialize the registers and buffers. */
  if( pInit )
  {
    tHALTLMMInit.nNumGpio = pInit->nNumGpio;
    tHALTLMMInit.nBaseAddress = pInit->nBaseAddress;

    if((tHALTLMMInit.nNumGpio != 0) && (tHALTLMMInit.nNumGpio <= HAL_TLMM_TOTAL_GPIO))
    {
      for(nIdx = 0; nIdx < tHALTLMMInit.nNumGpio; nIdx++)
      {
         uHAL_tlmm_Configs[nIdx].nCfgAddr += tHALTLMMInit.nBaseAddress;
         uHAL_tlmm_Configs[nIdx].nInOutAddr += tHALTLMMInit.nBaseAddress; 
      }
    }

    /*
     * Remove the GPIO1 from configurability as it is used for retention pin control.
     */
    uHAL_tlmm_Configs[1].nCfgAddr = 0;
    uHAL_tlmm_Configs[1].nInOutAddr = 0;
  }

}/* END uHAL_tlmm_Init */



/*===============================================================================

FUNCTION  
  void uHAL_tlmm_WriteConfig( uint32 nGpioNumber, uint32 nConfig )

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
void uHAL_tlmm_WriteConfig( uint32 nGpioNumber, uint32 nConfig )
{
  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    outpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr, nConfig);
  }
}


/*===============================================================================

FUNCTION  
   void uHAL_tlmm_Reset( void )

DESCRIPTION
   Will program the reset state of the TLMM block.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===============================================================================*/

void uHAL_tlmm_Reset( void )
{
  /* Reserved for future use. */
}


/*===============================================================================

FUNCTION  
  void uHAL_tlmm_ConfigGpio ( uint32 nGpioConfig )

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

void uHAL_tlmm_ConfigGpio( uint32 nGpioConfig )
{
  uint32 nGpioNumber = (uint32)HAL_GPIO_NUMBER(nGpioConfig);

  if( nGpioNumber < HAL_TLMM_TOTAL_GPIO )
  {
     /* If an output value was specified. */
    if( HAL_GPIO_OUTVAL(nGpioConfig) == HAL_TLMM_OUTPUT_LOW )
    {
      uHAL_tlmm_WriteGpio( nGpioConfig, FALSE );
    }
    else if( HAL_GPIO_OUTVAL(nGpioConfig) == HAL_TLMM_OUTPUT_HIGH )
    {
      uHAL_tlmm_WriteGpio( nGpioConfig, TRUE );
    } 
 
    uHAL_tlmm_WriteConfig(nGpioNumber, (uint32)HAL_GPIO_CONFIG_MASK(nGpioConfig));
  }

}

/*===============================================================================

FUNCTION  
   void uHAL_tlmm_WriteGpioGroup( uint32 pnGpioConfigArr[], uint16 nWhatSize )

DESCRIPTION
   Outputs a given value to a group of GPIOs based on the input parameters.

PARAMETERS
   pnGpioConfigArr[]  - The array of GPIO configurations to write to.
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
void uHAL_tlmm_WriteGpioGroup
(
  uint32 *pnGpioConfigArr, 
  uint32 nWhatSize,
  boolean bWriteVal
)
{
  uint32 nIdx;

  for( nIdx=0; nIdx< (uint32)nWhatSize; ++nIdx )
    uHAL_tlmm_WriteGpio(pnGpioConfigArr[nIdx], bWriteVal);
}


/*===============================================================================

FUNCTION  
  void uHAL_tlmm_ConfigGpioGroup ( uint32 pnGpioConfigArr[], 
                                  uint32 size )

DESCRIPTION
   Configures a group of GPIOs based on the supplied input parameters.  Each
   array contains a GPIO/Configuration set, but we extract only the GPIO from
   the first array and the configuration from the second.  This allows GPIO
   groups to be set to several different configurations, such as primary,
   sleep, or default group configuration.

PARAMETERS
   pnGpioConfigArr[]  - An array of GPIO configurations to extract the GPIO 
                      numbers to configure.
   nNumGpio        - The size of the first array.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===============================================================================*/
void uHAL_tlmm_ConfigGpioGroup
(
  uint32 *pnGpioConfigArr,
  uint32 nNumGpio
)
{
  uint32 nIdx;

  for( nIdx = 0; nIdx < nNumGpio; ++nIdx )
  {
    uHAL_tlmm_ConfigGpio(pnGpioConfigArr[nIdx]);
  }
}


/*===============================================================================
FUNCTION  
  void uHAL_tlmm_Save ( void )

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
void uHAL_tlmm_Save( void )
{
  /* Nothing to do here. */
}


/*===============================================================================
FUNCTION  
  void uHAL_tlmm_Restore( void )

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
void uHAL_tlmm_Restore( void )
{
  /* Nothing to do here. */
}


/*===============================================================================

FUNCTION  
  uHAL_tlmm_ReadGpio( uint32 nGpioConfig )

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
boolean uHAL_tlmm_ReadGpio( uint32 nGpioConfig )
{
  boolean retval = FALSE;
  uint32 nGpioNumber = (uint32)HAL_GPIO_NUMBER(nGpioConfig);

  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nInOutAddr )
  {
    if(inpdw(uHAL_tlmm_Configs[nGpioNumber].nInOutAddr) & 0x1)
      retval = TRUE;
  }
  return(retval);

}


/*===============================================================================

FUNCTION  
  void uHAL_tlmm_WriteGpio( uint32 nGpioConfig, bool bValue )

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
void uHAL_tlmm_WriteGpio( uint32 nGpioConfig, boolean bValue )
{
  uint32 nGpioNumber = (uint32)HAL_GPIO_NUMBER(nGpioConfig);

  if((nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nInOutAddr)
  {
    if(TRUE == bValue)
    {
      outpdw(uHAL_tlmm_Configs[nGpioNumber].nInOutAddr, 0x2);
    }
    else
    {
      outpdw(uHAL_tlmm_Configs[nGpioNumber].nInOutAddr, 0x0);
    }
  }
}

/*===============================================================================

FUNCTION  
  void uHAL_tlmm_GetOutput ( uint32 nGpioNumber, boolean value )

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
boolean uHAL_tlmm_GetOutput
(
  uint32 nGpioNumber
)
{
    dword  val = 0;
    /* Ensure GPIO number is valid. */
    if(( nGpioNumber < HAL_TLMM_TOTAL_GPIO ) && uHAL_tlmm_Configs[nGpioNumber].nInOutAddr )
    {
      val = inpdw(uHAL_tlmm_Configs[nGpioNumber].nInOutAddr) & 0x1;
      if( val )
      {
        return(TRUE);
      }
    }
    return(FALSE);
}


/*=========================================================================

FUNCTION  
  void uHAL_tlmm_SetPull( uint32 nGpioNumber, uHAL_tlmm_PullType nPull )

DESCRIPTION
   Updates the PULL value of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   ePull       - The uHAL_tlmm_PullType value to set to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void uHAL_tlmm_SetPull( uint32 nGpioNumber, uHAL_tlmm_PullType ePull )
{
  uint32 nValue = 0;

  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    nValue = inpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr);
    nValue &= ~(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_BMSK << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_SHFT);
    nValue |= (ePull << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_SHFT);

    outpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr, nValue);
  }

} /* END uHAL_tlmm_SetPull */


/*=========================================================================

FUNCTION  
  void uHAL_tlmm_SetDirection( uint32 nGpioNumber, uHAL_tlmm_DirType eDirection )

DESCRIPTION
   Changes the direction of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   eDirection  - The uHAL_tlmm_DirType value to change to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void uHAL_tlmm_SetDirection
(
  uint32 nGpioNumber,
  uHAL_tlmm_DirType eDirection
)
{
  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    uint32 nValue = inpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr);
 
    nValue &= ~(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_BMSK << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_SHFT);
    nValue |= (eDirection << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_SHFT);

    outpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr, nValue);
  }
} /* END uHAL_tlmm_SetDirection */


/*=========================================================================

FUNCTION  
  void uHAL_tlmm_SetDriveStrength( uint32 nGpioNumber, uHAL_tlmm_DriveType nPull )

DESCRIPTION
   Changes the drive strength of a GPIO.

PARAMETERS
   nGpioNumber - The GPIO nubmer to change the direction for.
   eDrive      - The uHAL_tlmm_DriveType value to change to.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void uHAL_tlmm_SetDriveStrength
(
  uint32 nGpioNumber,
  uHAL_tlmm_DriveType eDrive
)
{
  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    uint32 nValue = inpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr);
 
    nValue &= ~(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_BMSK << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_SHFT);
    nValue |= (eDrive << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_SHFT);

    outpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr, nValue);
  }

} /* END uHAL_tlmm_SetDriveStrength */


/*=========================================================================

FUNCTION  
  void uHAL_tlmm_SetFunction( uint32 nGpioNumber, uHAL_tlmm_DriveType nPull )

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

void uHAL_tlmm_SetFunction
(
  uint32 nGpioNumber,
  uint32 nFuncVal
)
{
  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    uint32 nValue = inpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr);
 
    nValue &= ~(HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_BMSK << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_SHFT);
    nValue |= (nFuncVal << HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_SHFT);

    outpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr, nValue);
  }

} /* END uHAL_tlmm_SetFunction */


/*=========================================================================

FUNCTION  
  void uHAL_tlmm_GetConfig( uint32* pnNumGpio )

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

void uHAL_tlmm_GetConfig
(
  uint32 nGpioNumber,
  uHAL_tlmm_GpioType* tGpio
)
{
  if( (nGpioNumber < HAL_TLMM_TOTAL_GPIO) && uHAL_tlmm_Configs[nGpioNumber].nCfgAddr )
  {
    uint32 nTempConfig = inpdw(uHAL_tlmm_Configs[nGpioNumber].nCfgAddr);

    tGpio->nFunc =  ((nTempConfig & HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_BMSK) >> HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_FUNC_SEL_SHFT);
    tGpio->nDir =   ((nTempConfig & HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_BMSK) >> HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_OE_SHFT);
    tGpio->nPull =  ((nTempConfig & HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_BMSK) >> HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_PULL_SHFT);
    tGpio->nDrive = ((nTempConfig & HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_BMSK) >> HWIO_SSC_TLMM_LITE_GPIO_CFG_0_GPIO_HDRIVE_SHFT);
  }
}

uint32 uHAL_tlmm_GetTotalGpio
(
  void
)
{
  return(HAL_TLMM_TOTAL_GPIO);
}

/*=============================================================================*/

