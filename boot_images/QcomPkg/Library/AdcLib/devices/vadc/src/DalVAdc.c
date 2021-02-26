/*============================================================================
  FILE:         DalVAdc.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the
                PMIC VADC peripheral.

  DEPENDENCIES: None

                Copyright (c) 2009-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/DalVAdc.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-23  jjo  Add HW supported checks.
  2015-08-12  jjo  Updated for VADC refresh.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#ifdef ADC_DIAG_LOGGING
#include "VAdcDiag.h"
#endif
#include "DDIPlatformInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define VADC_SPMI_READ_NUM_RETRIES 5

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
boolean gVAdcOverrideRumiCheck = FALSE;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdc_ReadBytes

  DESCRIPTION     This function reads from the VAdc peripheral

  DEPENDENCIES    None

  PARAMETERS
      pCtxt            [in]  pointer to the HAL interface context
      uOffset          [in]  the starting address
      pucData          [out] the bytes read
      uLen             [in]  the number of bytes to read

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType
VAdc_ReadBytes(
   void *pCtxt,
   uint32 uOffset,
   uint8 *pucData,
   uint32 uLen
   )
{
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pCtxt;
   uint32 uBytesRead;
   DALResult result;
   uint32 i;

   for (i = 0; i < VADC_SPMI_READ_NUM_RETRIES; i++)
   {
      result = AdcSpmiRead(&pDevCtxt->spmiCtxt,
                           uOffset,
                           pucData,
                           uLen,
                           &uBytesRead);
      if (result != DAL_SUCCESS || uBytesRead != uLen)
      {
         VAdc_LogError(&pDevCtxt->debug, "VAdc - SPMI read failed, try again", FALSE);
         continue;
      }

      return VADC_HAL_SUCCESS;
   }

   VAdc_LogError(&pDevCtxt->debug, "VAdc - SPMI read failed repeatedly", TRUE);
   return VADC_HAL_ERROR;
}

/*======================================================================

  FUNCTION        VAdc_WriteBytes

  DESCRIPTION     This function writes to the VAdc peripheral

  DEPENDENCIES    None

  PARAMETERS
      pCtxt            [in] pointer to the HAL interface context
      uOffset          [in] the starting address
      pucData          [in] the bytes to write
      uLen             [in] the number of bytes to write

  RETURN VALUE    VADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static VAdcHalResultType
VAdc_WriteBytes(
   void *pCtxt,
   uint32 uOffset,
   uint8 *pucData,
   uint32 uLen
   )
{
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pCtxt;
   DALResult result;

   result = AdcSpmiWrite(&pDevCtxt->spmiCtxt,
                         uOffset,
                         pucData,
                         uLen);
   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - SPMI write failed", TRUE);
      return VADC_HAL_ERROR;
   }

   return VADC_HAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_StartConversionInternal

  DESCRIPTION     This function starts the ADC conversion

  DEPENDENCIES    None

  PARAMETERS
      pDevCtxt          [in] pointer to the VADC device context
      pChConfig         [in] the channel configuration
      puConvTime_us     [out] conversion time

  RETURN VALUE    DAL_SUCCESS or an error code

  SIDE EFFECTS    DAL_SUCCESS if conversion was started successfully
                  Otherwise, returns a DAL error code.

======================================================================*/
static DALResult
VAdc_StartConversionInternal(
   VAdcDevCtxt *pDevCtxt,
   const VAdcChannelConfigType *pChConfig,
   uint32 *puConvTime_us
   )
{
   VAdcConversionParametersType conversionParams;
   VAdcHalResultType result;
#ifndef ADC_BOOT_DRIVER
   DALResult ret;
#endif

#ifndef ADC_BOOT_DRIVER
   /* Reset the EOC events prior to starting the conversion */
   ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT],
                          DALSYS_EVENT_CTRL_RESET);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   ret = DALSYS_EventCtrl(pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT],
                          DALSYS_EVENT_CTRL_RESET);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }
#endif

   /* Initialize conversion parameter variables */
   conversionParams.uChannel = pChConfig->uAdcHardwareChannel;
   conversionParams.eCalMethod = pChConfig->eCalMethod;
   conversionParams.eDecimationRatio = pChConfig->eDecimationRatio;
   conversionParams.eAverageMode = pChConfig->eAverageMode;
   conversionParams.eSettlingDelay = pChConfig->eSettlingDelay;

   result = VAdcHalRequestConversion(&pDevCtxt->iVAdcHalInterface,
                                     &conversionParams,
                                     puConvTime_us);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_WaitForEoc

  DESCRIPTION
      This function waits for the EOC and gets the conversion status.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt     [in]
      pChConfig    [in]
      uConvTime_us [in]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      Be careful not to make the wait time too small or EOC may
      time out too early.

======================================================================*/
static DALResult
VAdc_WaitForEoc(
   VAdcDevCtxt *pDevCtxt,
   const VAdcChannelConfigType *pChConfig,
   uint32 uConvTime_us
   )
{
   uint32 uReadTimeout_us = pDevCtxt->pBsp->uReadTimeout_us;
   VAdcStatusType convStatus;
   VAdcHalResultType status;
   uint32 uTimeWaited_us;
   uint32 uReadTime_us;
#ifndef ADC_BOOT_DRIVER
   DALResult result;
   uint32 uEventIdx;
#endif

   uReadTime_us = uConvTime_us + pDevCtxt->pBsp->uLDOSettlingTime_us;

#ifndef ADC_BOOT_DRIVER
   if (pDevCtxt->pBsp->bUsesInterrupts)
   {
      result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent,
                                        _VADC_NUM_EVENTS,
                                        uReadTimeout_us,
                                        &uEventIdx);
      if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
      {
         return result;
      }

      if (result == DAL_ERROR_TIMEOUT)
      {
         VAdc_LogError(&pDevCtxt->debug, "VAdc - missed interrupt", FALSE);
         VAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iVAdcHalInterface);
      }

      status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, &convStatus);
      if (status != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      if (convStatus.eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
      {
         VAdc_LogEOC(&pDevCtxt->debug);
         return DAL_SUCCESS;
      }
   }
   else
#endif
   {
      uTimeWaited_us = 0;

      while (uTimeWaited_us < uReadTimeout_us)
      {
#ifndef ADC_BOOT_DRIVER
         result = DALSYS_EventMultipleWait(pDevCtxt->ahEocEvent,
                                           _VADC_NUM_EVENTS,
                                           uReadTime_us,
                                           &uEventIdx);
         if (result != DAL_SUCCESS && result != DAL_ERROR_TIMEOUT)
         {
            return result;
         }
#else
         DALSYS_BusyWait(uReadTime_us);
#endif

         status = VAdcHalGetStatus(&pDevCtxt->iVAdcHalInterface, &convStatus);
         if (status != VADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }

         if (convStatus.eConversionStatus == VADC_CONVERSION_STATUS_COMPLETE)
         {
            VAdc_LogEOC(&pDevCtxt->debug);
            return DAL_SUCCESS;
         }

         if (uTimeWaited_us == 0)
         {
            uTimeWaited_us += uReadTime_us;

            /* Only wait the full read time for the first try and
               then poll at a faster rate */
            uReadTime_us = uReadTime_us / 5;
         }
         else
         {
            uTimeWaited_us += uReadTime_us;
         }
      }
   }

   /* Conversion did not complete */
   VAdc_LogError(&pDevCtxt->debug, "VAdc - conversion timed out", FALSE);
   VAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iVAdcHalInterface);

   return DAL_ERROR;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdcCode

  DESCRIPTION
      This function reads the ADC result.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt [in]
      puCode   [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
VAdc_ReadAdcCode(
   VAdcDevCtxt *pDevCtxt,
   uint32 *puCode
   )
{
   VAdcHalResultType result;
   uint32 uCode;

   result = VAdcHalGetConversionCode(&pDevCtxt->iVAdcHalInterface, &uCode);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (uCode == VADC_INVALID_ADC_CODE)
   {
      /* After every peripheral disable the data registers are set to an invalid code */
      VAdc_LogError(&pDevCtxt->debug, "VAdc - invalid ADC code read", FALSE);
      return DAL_ERROR;
   }
   else if (((int16)uCode) < 0)
   {
      /* Code is 16-bit signed and could read negative when close to zero */
      *puCode = 0;
   }
   else
   {
      *puCode = uCode;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_ReadAdc

  DESCRIPTION
      This function reads an ADC channel.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]
      pChConfig   [in]
      puCode      [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
VAdc_ReadAdc(
   VAdcDevCtxt *pDevCtxt,
   const VAdcChannelConfigType *pChConfig,
   uint32 *puCode
   )
{
   VAdcHalResultType status;
   uint32 uConvTime_us;
   DALResult result;

   result = VAdc_StartConversionInternal(pDevCtxt, pChConfig, &uConvTime_us);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   result = VAdc_WaitForEoc(pDevCtxt, pChConfig, uConvTime_us);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

   result = VAdc_ReadAdcCode(pDevCtxt, puCode);
   if (result != DAL_SUCCESS)
   {
      goto error;
   }

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
   VAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iVAdcHalInterface);
#endif

   status = VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_DISABLE);
   if (status != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;

error:
   VAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iVAdcHalInterface);
   (void)VAdcHalSetEnable(&pDevCtxt->iVAdcHalInterface, VADC_DISABLE);
   return result;
}

/*======================================================================

  FUNCTION        VAdc_ReadChannelInternal

  DESCRIPTION
      This function reads an ADC channel and returns the scaled result
      and status of the read. It is not meant for reading calibration
      channels.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uChannel         [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
VAdc_ReadChannelInternal(
   VAdcDevCtxt *pDevCtxt,
   uint32 uChannel,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   const VAdcChannelConfigType *pChConfig = &pDevCtxt->pBsp->paChannels[uChannel];
   DALResult result;
   uint32 uCode;

   result = VAdc_ReadAdc(pDevCtxt,
                         pChConfig,
                         &uCode);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   VAdcProcessConversionResult(&pChConfig->scalingFactor,
                               pChConfig->eScalingMethod,
                               pChConfig->pIntTable,
                               pChConfig->uPullUp,
                               pDevCtxt->pBsp->uFullScale_code,
                               pDevCtxt->pBsp->uFullScale_uV,
                               uCode,
                               pAdcDeviceResult);

   VAdc_LogConversionResult(&pDevCtxt->debug,
                            pChConfig->pszName,
                            pAdcDeviceResult);

#ifdef ADC_DIAG_LOGGING
   vadc_diag_send_client_log(uChannel,
                             pAdcDeviceResult,
                             (uint32)pChConfig->eDecimationRatio);
#endif

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        VAdc_InterruptCb

  DESCRIPTION
      This function is the callback to process the conversion complete
      interrupt from the arbiter. It sets the EOC signal.

  DEPENDENCIES    None

  PARAMETERS
      pArg            [in]
      uIntrStatusMask [in]

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
#ifndef ADC_BOOT_DRIVER
static void* VAdc_InterruptCb(
   void *pArg,
   uint32 uIntrStatusMask
   )
{
   VAdcDevCtxt *pDevCtxt = (VAdcDevCtxt *)pArg;
   DALResult result;
   VAdcHalResultType status;

   VAdc_LogInterrupt(&pDevCtxt->debug);

   /* Clear all pending interrupts */
   status = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface, VADC_INT_MASK_EOC);
   if (status != VADC_HAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to clear EOC interrupt", FALSE);
   }

   /* Set the EOC signal */
   result = DALSYS_EventCtrlEx(pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT],
                               DALSYS_EVENT_CTRL_TRIGGER,
                               0,
                               NULL,
                               0);
   if (result != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to set EOC signal", FALSE);
   }

   return NULL;
}
#endif

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * The following functions are for DALDriver specific functionality
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DriverInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
VAdc_DriverDeInit(VAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are declared in DalDevice Interface.
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DeviceInit(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALSYSPropertyVar propVar;
   VAdcHalResultType result;
   uint32 uChannelIdx;
   DALResult ret;
#ifndef ADC_BOOT_DRIVER
   const VAdcAveragedChannelConfigType *pAveragedChannelCfg;
   VAdcAveragedChannelType *pAveragedChannel;
   VAdcInterruptConfigType eInterruptConfig;
#endif
#ifndef VADC_UNIT_TEST
   const VAdcGpioChannelMappingType *pGpioChannelMapping;
   const VAdcChannelConfigType *pChConfig;
   uint32 uGpioMappingIdx;
   uint32 uGpioChannel;
   DALBOOL bGpioFound;
   pm_err_flag_type pmResult;
#endif

   ret = VAdc_LogInit(&pDevCtxt->debug, pDevCtxt->DevId);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   if (DALPLATFORMINFO_TYPE_RUMI == DalPlatformInfo_Platform() &&
       !gVAdcOverrideRumiCheck)
   {
      /* PMIC is usually not supported on RUMI */
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }
   else
   {
      pDevCtxt->bHardwareSupported = TRUE;
   }

   /* Read the DAL properties - hProp populated in device attach */
   ret = DALSYS_GetPropertyValue(pDevCtxt->hProp, "VADC_BSP", 0, &propVar);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to get BSP", TRUE);
      return ret;
   }

   pDevCtxt->pBsp = (VAdcBspType *)propVar.Val.pStruct;

#ifndef VADC_UNIT_TEST
   pmResult = pm_get_pmic_info(pDevCtxt->pBsp->ucPmicDevice, &pDevCtxt->pmicDeviceInfo);
   if (pmResult != PM_ERR_FLAG__SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to get PMIC device info", TRUE);
      return DAL_ERROR;
   }
#endif

#ifndef ADC_BOOT_DRIVER
   /* Create NPA clients */
   pDevCtxt->hNPACpuLatency = npa_create_sync_client("/core/cpu/latency/usec", "VADC", NPA_CLIENT_REQUIRED);
#endif

   /* Attach to to the SPMI driver */
   ret = AdcSpmiInit(&pDevCtxt->spmiCtxt,
                     pDevCtxt->pBsp->eAccessPriority,
                     pDevCtxt->pBsp->uSlaveId,
                     pDevCtxt->pBsp->uPeripheralId);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to attach to SPMI", TRUE);
      return ret;
   }

   /* Initialize the HAL interface */
   pDevCtxt->iVAdcHalInterface.pCtxt = (void *)pDevCtxt;
   pDevCtxt->iVAdcHalInterface.pfnWriteBytes = VAdc_WriteBytes;
   pDevCtxt->iVAdcHalInterface.pfnReadBytes = VAdc_ReadBytes;

   /* Verify the hardware meets the minimum version requirements */
   result = VAdcHalGetRevisionInfo(&pDevCtxt->iVAdcHalInterface, &pDevCtxt->revisionInfo);
   if (result != VADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (pDevCtxt->pBsp->ucPerphType != pDevCtxt->revisionInfo.ucType)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - Invalid VADC peripheral type", FALSE);
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }

   if (pDevCtxt->revisionInfo.usDigitalRev < pDevCtxt->pBsp->usMinDigRev)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - Invalid HW - dig rev", FALSE);
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }

   if (pDevCtxt->revisionInfo.usAnalogRev < pDevCtxt->pBsp->usMinAnaRev)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - Invalid HW - ana rev", FALSE);
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }

#ifndef ADC_BOOT_DRIVER
   if (pDevCtxt->pBsp->bUsesInterrupts)
   {
      ret = AdcSpmiRegisterInterrupt(&pDevCtxt->spmiCtxt,
                                     VADC_INT_MASK_EOC,
                                     VAdc_InterruptCb,
                                     pDevCtxt);
      if (ret != DAL_SUCCESS)
      {
         VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to register ISR", TRUE);
         return ret;
      }

      /* Configure peripheral interrupts */
      eInterruptConfig = VADC_INTERRUPT_CONFIG_RISING_EDGE;

      result = VAdcHalConfigInterrupts(&pDevCtxt->iVAdcHalInterface,
                                       VADC_INT_MASK_EOC,
                                       eInterruptConfig);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = VAdcHalSetInterruptMid(&pDevCtxt->iVAdcHalInterface,
                                      pDevCtxt->pBsp->uMasterID);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = VAdcHalClearInterrupts(&pDevCtxt->iVAdcHalInterface,
                                      VADC_INT_MASK_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = VAdcHalEnableInterrupts(&pDevCtxt->iVAdcHalInterface,
                                       VADC_INT_MASK_EOC);
      if (result != VADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   /* Configure the EOC event & timeout event */
   ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                            &pDevCtxt->ahEocEvent[VADC_EVENT_DEFAULT],
                            NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to create the EOC event", TRUE);
      return ret;
   }

   ret = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,
                            &pDevCtxt->ahEocEvent[VADC_EVENT_TIMEOUT],
                            NULL);
   if (ret != DAL_SUCCESS)
   {
      VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to create the EOC timeout event", TRUE);
      return ret;
   }
#endif

#ifndef VADC_UNIT_TEST
   /* Map static GPIOs */
   for (uChannelIdx = 0; uChannelIdx < *pDevCtxt->pBsp->puNumChannels; uChannelIdx++)
   {
      pChConfig = &pDevCtxt->pBsp->paChannels[uChannelIdx];
      bGpioFound = FALSE;

      for (uGpioMappingIdx = 0; uGpioMappingIdx < pDevCtxt->pBsp->uNumGpioChannelMappings; uGpioMappingIdx++)
      {
         pGpioChannelMapping = &pDevCtxt->pBsp->paGpioChannelMappings[uGpioMappingIdx];

         for (uGpioChannel = 0; uGpioChannel < pGpioChannelMapping->uNumChannels; uGpioChannel++)
         {
            if (pChConfig->uAdcHardwareChannel == pGpioChannelMapping->paucChannels[uGpioChannel])
            {
               bGpioFound = TRUE;
               break;
            }
         }

         if (bGpioFound)
         {
            pmResult = pm_gpio_set_ext_pin_config(pDevCtxt->pBsp->ucPmicDevice,
                                                  pGpioChannelMapping->eGpio,
                                                  PM_GPIO_EXT_PIN_DISABLE);
            if (pmResult != PM_ERR_FLAG__SUCCESS)
            {
               VAdc_LogError(&pDevCtxt->debug, "VAdc - failed to configure static GPIO", TRUE);
               return DAL_ERROR;
            }

            break;
         }
      }
   }
#endif

   /* Configure averaged channels */
   pDevCtxt->uTotalNumChannels = *pDevCtxt->pBsp->puNumChannels;

#ifndef ADC_BOOT_DRIVER
   if (*pDevCtxt->pBsp->puNumAveragedChannels != 0)
   {
      ret = DALSYS_Malloc((sizeof(VAdcAveragedChannelType) * *pDevCtxt->pBsp->puNumAveragedChannels),
                          (void **)&pDevCtxt->paAveragedChannels);
      if (ret != DAL_SUCCESS)
      {
         VAdc_LogError(&pDevCtxt->debug, "VAdc - could not allocate memory for averaged channels", TRUE);
         return ret;
      }

      for (uChannelIdx = 0; uChannelIdx < *pDevCtxt->pBsp->puNumAveragedChannels; uChannelIdx++)
      {
         pAveragedChannelCfg = &pDevCtxt->pBsp->paAveragedChannels[uChannelIdx];
         pAveragedChannel = &pDevCtxt->paAveragedChannels[uChannelIdx];

         pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_UNKNOWN;

         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszLowName,
                               &pAveragedChannel->uLowChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->debug, "VAdc - low averaged channel not found", TRUE);
            return ret;
         }

         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszMedName,
                               &pAveragedChannel->uMedChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->debug, "VAdc - med averaged channel not found", TRUE);
            return ret;
         }

         ret = VAdc_GetChannel(pCtxt,
                               pAveragedChannelCfg->pszHighName,
                               &pAveragedChannel->uHighChannel);
         if (ret != DAL_SUCCESS)
         {
            VAdc_LogError(&pDevCtxt->debug, "VAdc - high averaged channel not found", TRUE);
            return ret;
         }
      }

      pDevCtxt->uTotalNumChannels += *pDevCtxt->pBsp->puNumAveragedChannels;
   }
#endif

   return DAL_SUCCESS;
}

DALResult
VAdc_DeviceDeInit(VAdcClientCtxt *pCtxt)
{
   /* ADC is marked as static - deinit is never called */
   return DAL_SUCCESS;
}

DALResult
VAdc_Open(
   VAdcClientCtxt *pCtxt,
   uint32 dwaccessMode
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported)
   {
#ifndef ADC_BOOT_DRIVER
      if (pDevCtxt->hNPACpuLatency != NULL)
      {
         npa_issue_required_request(pDevCtxt->hNPACpuLatency, 1);
      }
#endif
   }

   return DAL_SUCCESS;
}

DALResult
VAdc_Close(VAdcClientCtxt *pCtxt)
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported)
   {
#ifndef ADC_BOOT_DRIVER
      if (pDevCtxt->hNPACpuLatency != NULL)
      {
         npa_complete_request(pDevCtxt->hNPACpuLatency);
      }
#endif
   }

   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are extended in DalAdcDevice Interface.
 *
 * These functions are documented in DDIAdcDevice.h
 * -------------------------------------------------------------------------*/
DALResult
VAdc_SetDeviceIndex(
   VAdcClientCtxt *pCtxt,
   uint32 uDeviceIndex
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   pDevCtxt->uDeviceIndex = uDeviceIndex;

   return DAL_SUCCESS;
}

DALResult
VAdc_GetDeviceProperties(
   VAdcClientCtxt *pCtxt,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported)
   {
      pAdcDeviceProp->uNumChannels = pDevCtxt->uTotalNumChannels;
   }
   else
   {
      pAdcDeviceProp->uNumChannels = 0;
   }

   pAdcDeviceProp->uNumMeas = 0;

   return DAL_SUCCESS;
}

DALResult
VAdc_GetChannel(
   VAdcClientCtxt *pCtxt,
   const char *pszChannelName,
   uint32 *puChannelIdx
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   const char *pszCurrentChannelName;
   uint32 uNumChannels;
   uint32 uChannel;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   uNumChannels = *pDevCtxt->pBsp->puNumChannels;

   for (uChannel = 0; uChannel < uNumChannels; uChannel++)
   {
      pszCurrentChannelName = pDevCtxt->pBsp->paChannels[uChannel].pszName;

      if (strcmp(pszChannelName, pszCurrentChannelName) == 0)
      {
         *puChannelIdx = uChannel;
         return DAL_SUCCESS;
      }
   }

#ifndef ADC_BOOT_DRIVER
   for (uChannel = 0; uChannel < *pDevCtxt->pBsp->puNumAveragedChannels; uChannel++)
   {
      pszCurrentChannelName = pDevCtxt->pBsp->paAveragedChannels[uChannel].pszName;

      if (strcmp(pszChannelName, pszCurrentChannelName) == 0)
      {
         *puChannelIdx = uChannel + uNumChannels;
         return DAL_SUCCESS;
      }
   }
#endif

   return DAL_ERROR;
}

DALResult
VAdc_ReadChannel(
   VAdcClientCtxt *pCtxt,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;
   DALResult result;
#ifndef ADC_BOOT_DRIVER
   const VAdcAveragedChannelConfigType *pAveragedChannelCfg;
   VAdcAveragedChannelType *pAveragedChannel;
   AdcDeviceResultType lowResult;
   AdcDeviceResultType medResult;
   AdcDeviceResultType highResult;
#endif

   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->uTotalNumChannels)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;
      return DAL_ERROR;
   }

   VAdc_LogConversionRequest(&pDevCtxt->debug, uChannelIdx);

#ifndef ADC_BOOT_DRIVER
   if (uChannelIdx >= *pDevCtxt->pBsp->puNumChannels)
   {
      /* Averaged channel reading */
      pAveragedChannelCfg = &pDevCtxt->pBsp->paAveragedChannels[uChannelIdx - *pDevCtxt->pBsp->puNumChannels];
      pAveragedChannel = &pDevCtxt->paAveragedChannels[uChannelIdx - *pDevCtxt->pBsp->puNumChannels];

      /* The medium channel is always read first. Decisions about when to transition
       * need to be made with the same channel. Otherwise, jumps can happen, as well
       * as getting different values going from low-to-high versus high-to-low */
      result = VAdc_ReadChannelInternal(pDevCtxt, pAveragedChannel->uMedChannel, &medResult);
      if (result != DAL_SUCCESS || medResult.eStatus != ADC_DEVICE_RESULT_VALID)
      {
         return DAL_ERROR;
      }

      if (medResult.nPhysical > pAveragedChannelCfg->nHighToMed)
      {
         result = VAdc_ReadChannelInternal(pDevCtxt, pAveragedChannel->uHighChannel, &highResult);
         if (result != DAL_SUCCESS || highResult.eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return result;
         }

         if (medResult.nPhysical < pAveragedChannelCfg->nMedToHigh)
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_MED_HIGH;
            VAdcBlendResult(medResult.nPhysical,
                            &medResult,
                            pAveragedChannelCfg->nHighToMed,
                            &highResult,
                            pAveragedChannelCfg->nMedToHigh,
                            pAdcDeviceResult);
         }
         else
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_HIGH;
            *pAdcDeviceResult = highResult;
         }
      }
      else if (medResult.nPhysical < pAveragedChannelCfg->nLowToMed)
      {
         result = VAdc_ReadChannelInternal(pDevCtxt, pAveragedChannel->uLowChannel, &lowResult);
         if (result != DAL_SUCCESS || lowResult.eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return result;
         }

         if (medResult.nPhysical > pAveragedChannelCfg->nMedToLow)
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_LOW_MED;
            VAdcBlendResult(medResult.nPhysical,
                            &lowResult,
                            pAveragedChannelCfg->nMedToLow,
                            &medResult,
                            pAveragedChannelCfg->nLowToMed,
                            pAdcDeviceResult);
         }
         else
         {
            pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_LOW;
            *pAdcDeviceResult = lowResult;
         }
      }
      else
      {
         pAveragedChannel->eState = VADC_AVERAGED_CHANNEL_STATE_MED;
         *pAdcDeviceResult = medResult;
      }
   }
   else
   {
#endif
      /* Normal channel reading */
      result = VAdc_ReadChannelInternal(pDevCtxt, uChannelIdx, pAdcDeviceResult);
      if (result != DAL_SUCCESS)
      {
         return result;
      }
#ifndef ADC_BOOT_DRIVER
   }
#endif

   return result;
}

DALResult
VAdc_RecalibrateChannel(
   VAdcClientCtxt *pCtxt,
   uint32 uChannelIdx,
   AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult
   )
{
   VAdcDevCtxt *pDevCtxt = pCtxt->pVAdcDevCtxt;

   pAdcDeviceRecalibrateResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->uTotalNumChannels)
   {
      pAdcDeviceRecalibrateResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;
      return DAL_ERROR;
   }

   /* For this PMIC design, calibration is taken care of automatically
      by the hardware so just return the scaling values */
   pAdcDeviceRecalibrateResult->eStatus = ADC_DEVICE_RESULT_VALID;
   pAdcDeviceRecalibrateResult->nPhysical1_uV = 0;
   pAdcDeviceRecalibrateResult->nPhysical2_uV = pDevCtxt->pBsp->uFullScale_uV;
   pAdcDeviceRecalibrateResult->uCode1 = 0;
   pAdcDeviceRecalibrateResult->uCode2 = pDevCtxt->pBsp->uFullScale_code;

   return DAL_SUCCESS;
}

