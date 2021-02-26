/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the uTlmm TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/tlmm/hw/slpi/common/uTlmm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   dcf     Fix for immediateConfigs in restoring GPIOs from sleep.
02/23/09   an      Mainlining FEATURE_MANAGE_GPIOS_FOR_SLEEP and 
                   FEATURE_APPS_GPIOS_LOW_POWER by having internal flags 
                   in TLMM.
01/08/09   dcf     General clean up, removal of bare enumerated types and 
                   addition of uniform headers and comments.
===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "uTlmm.h"
#include "uHALtlmm.h"
#include "uTlmmInternal.h"

/*==========================================================================

                     LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/

#define UTLMM_STATE_ACTIVE 0x80000000

extern uint32 uHAL_tlmm_GetTotalGpio( void );

#define UTLMM_SET_GPIO_NUMBER(cfg, pin) \
                (((cfg) &~ 0x3FF0) | ((pin) << 4))

/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/

static uTlmmInitType gnTlmmSettings;

/*==========================================================================

  FUNCTION      uTlmm_ConfigGpio

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean uTlmm_ConfigGpio
(  
  uTlmmGpioSignalType   eGpioConfig,
  uTlmmGpioEnableType   eEnableType
)
{
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);
  uint32 nLowPowerCfg;

  if(nGpioNumber < uHAL_tlmm_GetTotalGpio())
  {
    if( eEnableType == UTLMM_GPIO_ENABLE )
    {
      uHAL_tlmm_ConfigGpio(eGpioConfig);
      gnTlmmSettings.anLowPowerCfgs[nGpioNumber] |= UTLMM_STATE_ACTIVE;
    }
    else
    {
      nLowPowerCfg = gnTlmmSettings.anLowPowerCfgs[nGpioNumber];
      nLowPowerCfg = UTLMM_SET_GPIO_NUMBER(nLowPowerCfg, nGpioNumber);
      uHAL_tlmm_ConfigGpio(nLowPowerCfg);
      gnTlmmSettings.anLowPowerCfgs[nGpioNumber] &= ~UTLMM_STATE_ACTIVE;
    }
    return(TRUE);
  } 
  return(FALSE);

} /* uTlmm_ConfigGpio */


/*==========================================================================

  FUNCTION      uTlmm_GetGpioNumber

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean  uTlmm_GetGpioNumber
(
  uTlmmGpioSignalType eGpioConfig,
  uint32*            pnGpioNumber
) 
{
  if(pnGpioNumber != NULL)
  {
    *pnGpioNumber = (uint32)HAL_GPIO_NUMBER(eGpioConfig);
    return(TRUE);
  }
  return(FALSE);

} /* uTlmm_GetGpioNumber */


/*==========================================================================

  FUNCTION      uTlmm_GpioIn

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean  uTlmm_GpioIn
(
  uTlmmGpioSignalType  eGpioConfig,  
  uTlmmGpioValueType*  peValue
) 
{
  boolean bHalRetVal = FALSE;
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if(peValue != NULL)
  {
    if( nGpioNumber < uHAL_tlmm_GetTotalGpio() )
    {
      bHalRetVal = uHAL_tlmm_ReadGpio(eGpioConfig);
    }

    if( !bHalRetVal  )
    {  
      *peValue = UTLMM_GPIO_LOW_VALUE;
    }
    else
    {  
      *peValue = UTLMM_GPIO_HIGH_VALUE;
    }
    return(TRUE);
  }
  return(FALSE);

} /* uTlmm_GpioIn */


/*==========================================================================

  FUNCTION      uTlmm_GpioOut

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean uTlmm_GpioOut
( 
  uTlmmGpioSignalType  eGpioConfig,   
  uTlmmGpioValueType   eValue
) 
{
  boolean bValue = FALSE;

  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if(nGpioNumber < uHAL_tlmm_GetTotalGpio())
  {
    if(eValue == UTLMM_GPIO_HIGH_VALUE)
    {
      bValue = TRUE;
    }

    uHAL_tlmm_WriteGpio((uint32)eGpioConfig, bValue);
    return(TRUE);
  }
  return(FALSE);

} /* uTlmm_GpioOut */


/*==========================================================================

  FUNCTION      uTlmm_GetCurrentConfig

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean uTlmm_GetCurrentConfig
(
  uint32               nGpioNumber,
  uTlmmGpioSignalType* peGpioConfig
)
{
  uHAL_tlmm_GpioType tTempCurrent;

  if(peGpioConfig != NULL)
  {
    if( nGpioNumber < uHAL_tlmm_GetTotalGpio() )
    {
      /* 
       * Read the current configuration.
       */

      uHAL_tlmm_GetConfig(nGpioNumber, &tTempCurrent);

      /* 
       * Convert to uTlmmGpioSignalType format.
       */

      *peGpioConfig = UTLMM_GPIO_CFG(nGpioNumber,
                                     tTempCurrent.nFunc,
                                     tTempCurrent.nDir,
                                     tTempCurrent.nPull,
                                     tTempCurrent.nDrive);

      return(TRUE);
    }
  }
  return(FALSE);

} /* uTlmm_GetCurrentConfig */


/*==========================================================================

  FUNCTION      uTlmm_GetPinStatus

  DESCRIPTION   See uTlmm.h

==========================================================================*/ 

boolean uTlmm_GetPinStatus
(
  uint32               nGpioNumber,
  uTlmmGpioStatusType* peStatus
)
{
  if(peStatus != NULL)
  {
    if(nGpioNumber < uHAL_tlmm_GetTotalGpio())
    {
      if(UTLMM_STATE_ACTIVE & gnTlmmSettings.anLowPowerCfgs[nGpioNumber])
      {
        *peStatus = UTLMM_GPIO_ACTIVE;
      }
      else
      {
        *peStatus = UTLMM_GPIO_INACTIVE;
      }
      return(TRUE);
    }
  }
  return(FALSE);

} /* uTlmm_GetPinStatus */


/*==========================================================================

  FUNCTION      uTlmm_SetInactiveConfig

  DESCRIPTION   See uTlmm.h

==========================================================================*/

boolean uTlmm_SetInactiveConfig
(
  uint32              nGpioNumber,
  uTlmmGpioSignalType nInactiveConfig
)
{
  if( nGpioNumber < uHAL_tlmm_GetTotalGpio() )
  {
    /* 
     * Update the low-power configuration.
     */
    gnTlmmSettings.anLowPowerCfgs[nGpioNumber] = nInactiveConfig;

    return(TRUE);
  }
  return(FALSE);

} /* uTlmm_SetInactiveConfig */


/*==========================================================================

  FUNCTION      uTlmm_GetInactiveConfig

  DESCRIPTION   See uTlmm.h

==========================================================================*/

boolean uTlmm_GetInactiveConfig
(
  uint32               nGpioNumber,
  uTlmmGpioSignalType* peGpioConfig
)
{
  if(peGpioConfig != NULL)
  {
    if(nGpioNumber < uHAL_tlmm_GetTotalGpio())
    {
      *peGpioConfig = gnTlmmSettings.anLowPowerCfgs[nGpioNumber];
      return(TRUE);
    }
  }
  return(FALSE);

} /* uTlmm_GetInactiveConfig */


/*==========================================================================

  FUNCTION      uTlmm_GetOutput

  DESCRIPTION   See uTlmm.h

==========================================================================*/

boolean uTlmm_GetOutput
( 
  uint32              nGpioNumber, 
  uTlmmGpioValueType* pnValue
)
{
  if(pnValue != NULL)
  {
    if( nGpioNumber < uHAL_tlmm_GetTotalGpio() )
    {
      if(TRUE == uHAL_tlmm_GetOutput(nGpioNumber))
      {
        *pnValue = UTLMM_GPIO_HIGH_VALUE;
      }
      else
      {
        *pnValue = UTLMM_GPIO_LOW_VALUE;
      }
      return(TRUE);
    }
  }
  return(FALSE);

} /* uTlmm_GetOutput */


