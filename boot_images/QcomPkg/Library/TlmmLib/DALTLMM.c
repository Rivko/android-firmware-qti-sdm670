/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2010 - 2017 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


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


#include <HALtlmm.h>
#include <DALStdDef.h>
#include <DDIChipInfo.h>
#include "DALTLMMState.h"
#include "DALTLMM.h"
#include "ChipInfo.h"
#include "ChipInfoDefs.h"


/*==========================================================================

                     LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/


typedef struct
{
  uint32 ePort;
  uint32 nOffset;
  uint32 nMask;
  uint32 nEnableValue;
  uint32 nDisableValue;

}DALTLMM_PortCfgType;



/*==========================================================================

                     PROTOTYPES FOR THIS FILE

==========================================================================*/


DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt );

/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/
static boolean gbTLMMInitialized = FALSE;
uint32 gnTotalGPIO;
extern DALTLMMStateType* pgtState;
static DALTLMM_PortCfgType gtHALPorts[DAL_TLMM_NUM_PORTS];
boolean bGpioInUse[200];
static uint32 gnTotalPorts = 0;
static boolean gNonDalTlmmInit = FALSE;
uint32* pgLowPowerConfigs = NULL;
DALSYS_PROPERTY_HANDLE_DECLARE(hNonDalProps);

/*==========================================================================

      GENERIC DAL FUNCTIONS NOT CURRENTLY IMPLEMENTED IN THIS MODULE

==========================================================================*/

DALResult 
TLMM_DriverInit(TLMMDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
TLMM_DriverDeInit(TLMMDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
TLMM_PowerEvent(TLMMClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Open(TLMMClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Close(TLMMClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Info(TLMMClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}


/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/



/*==========================================================================

  FUNCTION      TLMM_DeviceInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceInit(TLMMClientCtxt *pCtxt)
{
  DALSYSPropertyVar tPropVar;
  DALResult eResult = DAL_ERROR;
  uint32 nPortIdx = 0, nHALPort = 0;
  uint32 nIdx = 0;
 
  if( !gbTLMMInitialized )
  {
    if ( DAL_SUCCESS != ChipInfo_Init() )
    {
      return DAL_ERROR;
    } 

    for( nIdx=0; nIdx<DAL_TLMM_NUM_PORTS; nIdx++ )
    {
      gtHALPorts[nIdx].ePort = DAL_TLMM_NUM_PORTS;
    }
    /* 
     * Initialize the synchronization.
     */

    if(DAL_SUCCESS != DALSYS_SyncCreate(
                                      DALSYS_SYNC_ATTR_RESOURCE,
                                      &(pCtxt->pDevCtxt->hTlmmSync),
                                      &(pCtxt->pDevCtxt->hTlmmSyncObj)))
    {
        return(DAL_ERROR);
    }

    /* 
     * Ensure synchronization for critical initialization parameters.
     */

    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    /* 
     * Attempt to retrieve a handle to the TLMM properties file.
     */

    eResult = DALSYS_GetDALPropertyHandle(pCtxt->pDevCtxt->DevId, pCtxt->pDevCtxt->hProp);

    if( DAL_SUCCESS != eResult )
    {
      /*
       * This is fatal.
       */
      return( DAL_ERROR );
    }

    HAL_tlmm_GetNumberGpio(&gnTotalGPIO);


    /* 
     * Attempt to retrieve any port configuration structures.
     */

    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_ports", 0, &tPropVar );

    if( DAL_SUCCESS == eResult )
    {
      while( tPropVar.Val.pdwVal[nPortIdx] != 0xFFFFFFFF )
      {
        nHALPort = tPropVar.Val.pdwVal[nPortIdx++];

        if( nHALPort < DAL_TLMM_NUM_PORTS )
        {
        gtHALPorts[nHALPort].ePort = nHALPort;
        gtHALPorts[nHALPort].nOffset = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nMask = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nEnableValue = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nDisableValue = tPropVar.Val.pdwVal[nPortIdx++];
        gnTotalPorts++;
      }
    }
    }

      /* 
       * Initialize the HAL interface.  Nothing to do.  Already initialized.
       */
  
    HAL_tlmm_Init(NULL);

    eResult = TLMM_PostInit(pCtxt);
    
    if ( eResult != DAL_SUCCESS )
    {
      return eResult;
    }

    gbTLMMInitialized = TRUE;

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

  }

  return(DAL_SUCCESS);

} /* TLMM_DeviceInit */


/*==========================================================================

  FUNCTION      TLMM_PostInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt )
{
  DALSYSPropertyVar tPropVar;
  uint32 nSize = (sizeof(DALTLMMStateType)+ (sizeof(DALTLMMStateGPIOType)*gnTotalGPIO));
  DALResult eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "lpmode_hw_based", 0, &tPropVar );

  if( DAL_SUCCESS == eResult )
  {
    if (DALSYS_Malloc(nSize, (void**)&pgtState) != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
    memset(pgtState, 0x0, nSize);
  }
  else
  {
  if(DAL_SUCCESS == DALGLBCTXT_Init())
  {
    /* Initialize or retrieve a pointer to shared memory. */
    
      if( DAL_SUCCESS != DALGLBCTXT_AllocCtxt("DALTLMM",
                                 (sizeof(DALTLMMStateType)+ 
                                  (sizeof(DALTLMMStateGPIOType)*gnTotalGPIO)), 
                                  0x0, 
                                 (void **)&pgtState))
    {
        return DAL_ERROR;
      } 
    }      
  }

      if( pgtState )
      {
        /* 
         * Ensure local driver has initialized.
         */
        if( !pgtState->tFlags.bDriverInited )
        {
          DALTLMMState_Init(pCtxt);
        }
   
        pgtState->tFlags.bSMInited = TRUE;
        return(DAL_SUCCESS);
      }

  return(DAL_ERROR);

} /* TLMM_PostInit */


/*==========================================================================

  FUNCTION      TLMM_DeviceDeInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceDeInit
(
  TLMMClientCtxt *pCtxt
)
{
  if(DAL_SUCCESS == DALSYS_DestroyObject(pCtxt->pDevCtxt->hTlmmSync))
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }

} /* TLMM_DeviceDeInit */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_ConfigGpio
(
  TLMMClientCtxt*     pCtxt,   
  DALGpioSignalType   eGpioConfig,
  DALGpioEnableType   eEnable
)
{
  return(TLMM_ConfigGpioGroup(pCtxt, eEnable, &eGpioConfig, 1));

} /* TLMM_ConfigGpio */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioGroup

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_ConfigGpioGroup
(
  TLMMClientCtxt*     pCtxt,
  DALGpioEnableType   eEnable,
  DALGpioSignalType*  eGpioGroup,
  uint32              nSize
)
{
  DALResult eResult = DAL_ERROR;

  uint32 nIdx;
  DALGpioSignalType nCfg;
  uint32 nGpioNumber;

  (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

  switch(eEnable)
  {
    case DAL_TLMM_GPIO_ENABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        if( nGpioNumber < gnTotalGPIO )
        {
          /* 
           * Program the hardware.
           */
          HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);

          bGpioInUse[nGpioNumber] = TRUE;

          if( pgtState )
          {
            pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 1;
          }
        }
      }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        if( (nGpioNumber < gnTotalGPIO) && pgtState )
        {
          /*
           * Convert the configuration to macro format.
           */

          nCfg = DALTLMMState_StructToMacro(nGpioNumber,
                                            &pgtState->ptGPIO[nGpioNumber].tInactive);

          /* 
           * Write any low-power output values.
           */

          TLMM_GpioOut(pCtxt,
                     eGpioGroup[nIdx],
                     (DALGpioValueType)pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal);

          /* 
           * Program the hardware.
           */

          HAL_tlmm_ConfigGpio(nCfg);

          bGpioInUse[nGpioNumber] = FALSE;

          pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 0;
        }
      }
      eResult = DAL_SUCCESS;
      break;

    default:
      eResult = DAL_ERROR;
      break;

  }

  (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

  return(eResult);

} /* TLMM_ConfigGpioGroup */


/*==========================================================================

  FUNCTION      TLMM_SetPort

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_SetPort
( 
  TLMMClientCtxt*  pCtxt,   
  DALGpioPortType  ePort,   
  uint32           nValue
)
{
  DALResult eResult = DAL_ERROR;

  if( (ePort < DAL_TLMM_NUM_PORTS) && (gtHALPorts[ePort].ePort != DAL_TLMM_NUM_PORTS) )
  {
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    if( nValue != 0 )
    {
      HAL_tlmm_SetPort(gtHALPorts[ePort].nOffset, 
                       gtHALPorts[ePort].nMask, 
                       gtHALPorts[ePort].nEnableValue);
    }
    else
    {
      HAL_tlmm_SetPort(gtHALPorts[ePort].nOffset, 
                       gtHALPorts[ePort].nMask, 
                       gtHALPorts[ePort].nDisableValue);
    }

    eResult = DAL_SUCCESS;

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
  }

  return(eResult);

} /* TLMM_SetPort */


/*==========================================================================

  FUNCTION      TLMM_GpioIn

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioIn
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig,  
  DALGpioValueType*  eValue
) 
{
  DALResult eResult = DAL_ERROR;
 DALBOOL bHalRetVal = FALSE;
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if( nGpioNumber < gnTotalGPIO )
  {
    bHalRetVal = HAL_tlmm_ReadGpio(eGpioConfig);
    eResult = DAL_SUCCESS;
  }
  
  if( !bHalRetVal  )
  {  
    *eValue = DAL_GPIO_LOW_VALUE;
  }
  else
  {  
    *eValue = DAL_GPIO_HIGH_VALUE;
  }

  return(eResult);

} /* TLMM_GpioIn */


/*==========================================================================

  FUNCTION      TLMM_GpioOut

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioOut
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig,   
  DALGpioValueType   eValue
) 
{
  return(TLMM_GpioOutGroup(pCtxt, &eGpioConfig, 1, eValue));

} /* TLMM_GpioOut */


/*==========================================================================

  FUNCTION      TLMM_GpioOutGroup

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioOutGroup
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType* aeGpioConfigGroup,  
  uint32             nSize,
  DALGpioValueType   eValue
)
{
  DALResult eResult = DAL_ERROR;

  if( aeGpioConfigGroup )
  { 
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    HAL_tlmm_WriteGpioGroup((const uint32*)aeGpioConfigGroup,
                            (uint16)nSize,
                           (DALBOOL)eValue);

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GpioOutGroup */


/*==========================================================================

  FUNCTION      TLMM_SetInactiveConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_SetInactiveConfig
(
  TLMMClientCtxt*   pCtxt,
  uint32            nGpioNumber,
  DALGpioSignalType nInactiveConfig
)
{
  if( (nGpioNumber < gnTotalGPIO) && pgtState )
  {
    /* 
     * Update the low-power configuration.
     */

    DALTLMMState_MacroToStruct(nInactiveConfig, 
                               &pgtState->ptGPIO[nGpioNumber].tInactive);

    /* 
     * Update the output value for the low-power configuration.
     */

    pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal = 
                               DAL_GPIO_GET_OUTVAL(nInactiveConfig);

    return(DAL_SUCCESS);
  }

  return(DAL_ERROR);

} /* TLMM_SetInactiveConfig */


boolean Tlmm_Init
(
   void
)
{
  DALSYS_GetDALPropertyHandle(DALDEVICEID_TLMM, hNonDalProps);

  if ( DAL_SUCCESS != ChipInfo_Init() )
  {
    return DAL_ERROR;
  } 

  HAL_tlmm_Init(NULL);

  HAL_tlmm_GetNumberGpio(&gnTotalGPIO);

  gNonDalTlmmInit = TRUE;

  return( TRUE );

}

DALResult  Tlmm_GpioIn
( 
  DALGpioSignalType  eGpioConfig,  
  DALGpioValueType*  eValue
)
{
  DALResult eResult = DAL_ERROR;
  DALBOOL bHalRetVal;
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if( eValue == NULL )
  {
    return(DAL_ERROR);
  }

  if( gNonDalTlmmInit == FALSE )
  {
    if(FALSE == Tlmm_Init())
    {
      return(DAL_ERROR);
    }
  }

  if( nGpioNumber < gnTotalGPIO )
  {
    bHalRetVal = HAL_tlmm_ReadGpio(eGpioConfig);

    if( !bHalRetVal  )
    {
      *eValue = DAL_GPIO_LOW_VALUE;
    }
    else
    {
      *eValue = DAL_GPIO_HIGH_VALUE;
    }

    eResult = DAL_SUCCESS;
  }

  return(eResult);

} /* Tlmm_GpioIn */

DALResult Tlmm_ConfigGpioGroup
(
  DALGpioEnableType  eEnable,
  DALGpioSignalType* eGpioGroup, 
  uint32             nSize
)
{
  uint32 nIdx;
  uint32 nGpioNumber = 0;
  static DALSYSPropertyVar tPropVar;
  DALResult eResult = DAL_ERROR;

  if( eGpioGroup == NULL )
  {
    return(DAL_ERROR);
  }

  if( gNonDalTlmmInit == FALSE )
  {
    if(FALSE == Tlmm_Init())
    {
      return(DAL_ERROR);
    }
  }

  switch(eEnable)
  {
    case DAL_TLMM_GPIO_ENABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);
        /* 
         * Program the hardware.
         */
        HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);

        bGpioInUse[nGpioNumber] = TRUE;

      }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      if( pgLowPowerConfigs == NULL )
      {
        eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_sleep", 0, &tPropVar );
        if( eResult == DAL_ERROR )
        {
          return(DAL_ERROR);
        }
        pgLowPowerConfigs = (uint32 *) tPropVar.Val.pdwVal;
      }
      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        /* 
         * Program the hardware.
         */
        HAL_tlmm_ConfigGpio(((nGpioNumber & 0x3FF) << 4) | pgLowPowerConfigs[nGpioNumber]);

        bGpioInUse[nGpioNumber] = FALSE;
      }
      eResult = DAL_SUCCESS;
      break;

    default:
      eResult = DAL_ERROR;
      break;
  }
  return(eResult);

}


DALResult TLMM_GetCurrentConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* peGpioConfig
)
{
  return(DAL_SUCCESS);
}

DALResult TLMM_GetInactiveConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* peGpioConfig
)
{
  return(DAL_SUCCESS);
}

DALResult TLMM_GetOutput
(
  TLMMClientCtxt* pCtxt,
  uint32 nGpioNumber,
  DALGpioValueType* peValue
)
{
  return(DAL_SUCCESS);
}


/*==========================================================================

  FUNCTION      TLMM_GetGpioNumber

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GetGpioNumber
( 
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType  eGpioConfig,
  uint32*            nGpioNumber
) 
{
  *nGpioNumber = (uint32)HAL_GPIO_NUMBER(eGpioConfig);
  return(DAL_SUCCESS);

} /* TLMM_GetGpioNumber */


/*==========================================================================

  FUNCTION      TLMM_GetGpioStatus

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GetGpioStatus
( 
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType  eGpioConfig,
  DALGpioStatusType* peStatus
) 
{
  uint32 nGpioNumber = (uint32)HAL_GPIO_NUMBER(eGpioConfig);

  if( (nGpioNumber < gnTotalGPIO) && pgtState )
  {
    if(pgtState->ptGPIO[nGpioNumber].tAttr.nActive == 1)
    {
      *peStatus = DAL_GPIO_ACTIVE;
    }
    else
    {
      *peStatus = DAL_GPIO_INACTIVE;
    }
  }
  return(DAL_SUCCESS);

} /* TLMM_GetGpioStatus */

