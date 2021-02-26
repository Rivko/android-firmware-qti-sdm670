/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2009-2015 Qualcomm Technologies, Inc. 
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
#include "DALTLMMState.h"
#include "DALTLMM.h"
#include "ChipInfo.h"

/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/

uint32 gnTotalGPIO;
extern DALTLMMStateType* pgtState;

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

DALTLMMStateType gnState[200];


/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/



/*==========================================================================

  FUNCTION      TLMM_DeviceInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceInit(TLMMClientCtxt *pCtxt)
{
  char* pszHALVersion = NULL;
  DALResult eResult = DAL_ERROR;

  /*
   * Check for shared memory.  If already initialized, get a pointer and 
   * we're done.
   */
 
  if( !pgtState )
  {
    if ( DAL_SUCCESS != ChipInfo_Init() )
    {
      return DAL_ERROR;
    }
    
    /* Call HAL to retrieve the number of GPIO on this target. */
    HAL_tlmm_GetNumberGpio(&gnTotalGPIO);

    /* Try to allocate or attach to shared memory. */
    eResult = TLMM_PostInit(pCtxt);

    if((DAL_SUCCESS != eResult) || (!pgtState))
    {  
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to initialize shared memory data structure.");

      return(DAL_ERROR);
    }

    /* Initialize the synchronization. */
    if(DAL_SUCCESS != DALSYS_SyncCreate(
                                      DALSYS_SYNC_ATTR_RESOURCE,
                                      &(pCtxt->pDevCtxt->hTlmmSync),
                                      &(pCtxt->pDevCtxt->hTlmmSyncObj)))
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                        DALSYS_LOGEVENT_FATAL_ERROR, 
                        "TLMM_DeviceInit: Failed to initialize sychronization.");
      return(DAL_ERROR);
    }

    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    /* Initialize the HAL interface. */
    HAL_tlmm_Init(&pszHALVersion);
  
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
  if(DAL_SUCCESS == DALGLBCTXT_Init())
    {
    /* Initialize or retrieve a pointer to shared memory. */
    if( DAL_SUCCESS == DALGLBCTXT_AllocCtxt("DALTLMM",
                                 (sizeof(DALTLMMStateType)+ 
                                  (sizeof(DALTLMMStateGPIOType)*gnTotalGPIO)), 
                                  0x0, 
                                 (void **)&pgtState))
    {
      /* 
       * If shared structure was not alrady initialized.
       */

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
    return( DAL_SUCCESS );
      }
      else
      {
        /* 
         * Pointer to data structure is NULL.
         */
        return(DAL_ERROR);
      }
    }
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
    DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                      "TLMM_DeviceDeInit: Failed to deallocate TLMM device");
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
          /* Program the hardware. */
          HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);  
          pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 1;
        }
      }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        if( nGpioNumber < gnTotalGPIO )
        {
          /* Convert the configuration to macro format. */
          nCfg = DALTLMMState_StructToMacro(nGpioNumber,
                                            &pgtState->ptGPIO[nGpioNumber].tInactive);

          /* Write any low-power output values. */
          TLMM_GpioOut(pCtxt,
                     eGpioGroup[nIdx],
                     (DALGpioValueType)pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal);

          /* Program the hardware. */
          HAL_tlmm_ConfigGpio(nCfg);
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
  HAL_tlmm_PortType eHalPortID;

  switch(ePort)
  {
    case DAL_TLMM_LCDC_CFG:	
      eHalPortID = HAL_TLMM_LCDC_CFG;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_USB_PORT_SEL:	
      eHalPortID = HAL_TLMM_USB_PORT_SEL;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_PORT_TSIF:	
      eHalPortID = HAL_TLMM_PORT_TSIF;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_PORT_CONFIG_USB:
      eHalPortID = HAL_TLMM_PORT_CONFIG_USB;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_AUD_PCM:
      eHalPortID = HAL_TLMM_AUD_PCM;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_UART1:
      eHalPortID = HAL_TLMM_UART1;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_UART3:
      eHalPortID = HAL_TLMM_UART3;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_KEYSENSE_SC_IRQ:
      eHalPortID = HAL_TLMM_KEYSENSE_SC_IRQ;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_KEYSENSE_A9_IRQ:
      eHalPortID = HAL_TLMM_KEYSENSE_A9_IRQ;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_TCXO_EN:
      eHalPortID = HAL_TLMM_TCXO_EN;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_SSBI_PMIC:
      eHalPortID = HAL_TLMM_SSBI_PMIC;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_PA_RANGE1:	
      eHalPortID = HAL_TLMM_PA_RANGE1;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_PAD_ALT_FUNC_SDIO_EN:	
      eHalPortID = HAL_TLMM_PAD_ALT_FUNC_SDIO_EN;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_SPARE_WR_PA_ON:
      eHalPortID = HAL_TLMM_SPARE_WR_PA_ON;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_SDC3_CTL:
      eHalPortID = HAL_TLMM_SDC3_CTL;
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_SDC4_CTL:
      eHalPortID = HAL_TLMM_SDC4_CTL;
      eResult = DAL_SUCCESS;
      break;

    default:
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                      DALSYS_LOGEVENT_INFO, 
                      "TLMM_SetPort: Invalid Port ID");
      return(DAL_ERROR);     
    }
  
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    HAL_tlmm_SetPort(eHalPortID, nValue, nValue);

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

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

  FUNCTION      TLMM_GetCurrentConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_GetCurrentConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* eGpioConfig
)
{
  DALResult eResult = DAL_ERROR;
  HAL_tlmm_GpioType tTempCurrent;

  if( nGpioNumber < gnTotalGPIO )
  {
    /* Read the current configuration. */
    HAL_tlmm_GetConfig(nGpioNumber, &tTempCurrent);

    /* Convert to DALGpioSignalType format. */
    *eGpioConfig = DALTLMMState_StructToMacro(nGpioNumber, &tTempCurrent);
    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GetCurrentConfig */


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
  if( nGpioNumber < gnTotalGPIO )
  {
    /* Update the low-power configuration. */
    DALTLMMState_MacroToStruct(nInactiveConfig, 
                               &pgtState->ptGPIO[nGpioNumber].tInactive);

    /* Update the output value for the low-power configuration. */
    pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal = 
                               DAL_GPIO_GET_OUTVAL(nInactiveConfig);

    return(DAL_SUCCESS);
  }

  return(DAL_ERROR);

} /* TLMM_SetInactiveConfig */


/*==========================================================================

  FUNCTION      TLMM_GetInactiveConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetInactiveConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* eGpioConfig
)
{
  DALTLMMStateGPIOType* ptState = NULL;

  if( nGpioNumber < gnTotalGPIO )
  {
    ptState = &pgtState->ptGPIO[nGpioNumber];

    /* Need to explicitly convert because of added nSleepVal.*/
    *eGpioConfig = 
      (DALGpioSignalType)DAL_GPIO_CFG_OUT(nGpioNumber,
                                          ptState->tInactive.nFunc,
                                          ptState->tInactive.nDir,
                                          ptState->tInactive.nPull,
                                          ptState->tInactive.nDrive,
                                          ptState->tAttr.nSleepVal);

    return(DAL_SUCCESS);
  }
  return(DAL_ERROR);

} /* TLMM_GetInactiveConfig */


/*==========================================================================

  FUNCTION      TLMM_GetOutput

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetOutput
( 
  TLMMClientCtxt*   pCtxt, 
  uint32            nGpioNumber, 
  DALGpioValueType* nValue
)
{
  if( nGpioNumber < gnTotalGPIO )
  {
    if(HAL_tlmm_GetOutput(nGpioNumber)) 
      *nValue = DAL_GPIO_HIGH_VALUE;
    else
      *nValue = DAL_GPIO_LOW_VALUE;
    return(DAL_SUCCESS);
  }
  return(DAL_ERROR);

} /* TLMM_GetOutput */


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
