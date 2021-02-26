/*============================================================================
  FILE:         DalFgAdcFwk.c

  OVERVIEW:     FGADC DAL framework file.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/DalFgAdcFwk.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-03-13  jjo  Forced batt id workaround.
  2016-04-22  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdcDevice.h"
#include "DalFgAdc.h"
#include "DALSys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
FgAdcDrvCtxt gFgAdcDrvCtxt =
{
   .FgAdcDALVtbl  = {FgAdc_DriverInit, FgAdc_DriverDeInit},
   .dwNumDev      = FGADC_MAX_NUM_DEVICES,
   .dwSizeDevCtxt = sizeof(FgAdcDevCtxt),
   .bInit         = 0,
   .dwRefs        = 0,
   .FgAdcDevCtxt  = {{0}}
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32
FgAdc_DalAdcDevice_AddRef(DalAdcDeviceHandle *h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32
FgAdc_DalAdcDevice_Detach(DalDeviceHandle *h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
FgAdc_DalAdcDevice_Init(DalDeviceHandle *h)
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_DeviceInit(pCtxt);
}

static DALResult
FgAdc_DalAdcDevice_DeInit(DalDeviceHandle *h)
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_DeviceDeInit(pCtxt);
}

static DALResult
FgAdc_DalAdcDevice_PowerEvent(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain
   )
{
   return DAL_SUCCESS;
}

static DALResult
FgAdc_DalAdcDevice_Open(
   DalDeviceHandle *h,
   uint32 mode
   )
{
   return FgAdc_Open(h->pClientCtxt, mode);
}

static DALResult
FgAdc_DalAdcDevice_Close(DalDeviceHandle *h)
{
   return FgAdc_Close(h->pClientCtxt);
}

static DALResult
FgAdc_DalAdcDevice_Info(
   DalDeviceHandle *h,
   DalDeviceInfo* info,
   uint32 infoSize
   )
{
   info->Version = DALADCDEVICE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
FgAdc_DalAdcDevice_SysRequest(
   DalDeviceHandle *h,
   DalSysReq ReqIdx,
   const void *SrcBuf,
   uint32 SrcBufLen,
   void *DestBuf,
   uint32 DestBufLen,
   uint32 *DestBufLenReq
   )
{
   return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdcDevice Interface.
------------------------------------------------------------------------------*/
static DALResult
FgAdc_DalAdcDevice_SetDeviceIndex(
   DalDeviceHandle *h,
   uint32 uDeviceIndex
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_SetDeviceIndex(pCtxt,
                               uDeviceIndex);
}

static DALResult
FgAdc_DalAdcDevice_GetInputProperties(
   DalDeviceHandle *h,
   const char *pChannelName,
   uint32 *puChannelIdx
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_GetChannel(pCtxt,
                           pChannelName,
                           puChannelIdx);
}

static DALResult
FgAdc_DalAdcDevice_GetDeviceProperties(
   DalDeviceHandle *h,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_GetDeviceProperties(pCtxt, pAdcDeviceProp);
}

#ifndef ADC_LOADER_DRIVER
static DALResult
FgAdc_DalAdcDevice_RecalibrateChannel(
   DalDeviceHandle *h,
   uint32 uChannelIdx,
   AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult
   )
{
   return DAL_ERROR;
}
#endif

static DALResult
FgAdc_DalAdcDevice_ReadChannel(
   DalDeviceHandle *h,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceReadResult
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_ReadChannel(pCtxt,
                            uChannelIdx,
                            pAdcDeviceReadResult);
}

#ifndef ADC_BOOT_DRIVER
static DALResult
FgAdc_DalAdcDevice_TMGetInputProperties(
   DalDeviceHandle*h,
   const char *pInputName,
   uint32 *puMeasIdx
   )
{
   return ADC_DEVICE_ERROR_TM_NOT_SUPPORTED;
}

static DALResult
FgAdc_DalAdcDevice_TMGetRange(
   DalDeviceHandle *h,
   uint32 uMeasIdx,
   AdcDeviceTMRangeType *pAdcDeviceTMRange
   )
{
   return ADC_DEVICE_ERROR_TM_NOT_SUPPORTED;
}

static DALResult
FgAdc_DalAdcDevice_TMSetThreshold(
   DalDeviceHandle *h,
   uint32 uMeasIdx,
   const DALSYSEventHandle hEvent,
   AdcDeviceTMThresholdType eThreshold,
   const int32 *pnThresholdDesired,
   int32 *pnThresholdSet
   )
{
   return ADC_DEVICE_ERROR_TM_NOT_SUPPORTED;
}
#endif

static DALResult
FgAdc_DalAdcDevice_FGSetThreshold(
   DalDeviceHandle *h,
   AdcDeviceFGThresholdType eWhichThresh,
   int32 nThreshold
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_SetThreshold(pCtxt,
                             eWhichThresh,
                             nThreshold);
}

static DALResult
FgAdc_DalAdcDevice_FGSetBattIdForced(
   DalDeviceHandle *h,
   DALBOOL bEnable
   )
{
   FgAdcClientCtxt *pCtxt = h->pClientCtxt;

   return FgAdc_SetBattIdForced(pCtxt,
                                bEnable);
}

static void
FgAdc_InitInterface(FgAdcClientCtxt *pclientCtxt)
{
   static const DalAdcDevice vtbl =
   {
      {
         FgAdc_DalAdcDevice_Attach,
         FgAdc_DalAdcDevice_Detach,
         FgAdc_DalAdcDevice_Init,
         FgAdc_DalAdcDevice_DeInit,
         FgAdc_DalAdcDevice_Open,
         FgAdc_DalAdcDevice_Close,
         FgAdc_DalAdcDevice_Info,
         FgAdc_DalAdcDevice_PowerEvent,
         FgAdc_DalAdcDevice_SysRequest
      },
      FgAdc_DalAdcDevice_SetDeviceIndex,
      FgAdc_DalAdcDevice_GetInputProperties,
      FgAdc_DalAdcDevice_GetDeviceProperties,
#ifndef ADC_LOADER_DRIVER
      FgAdc_DalAdcDevice_RecalibrateChannel,
#endif
      FgAdc_DalAdcDevice_ReadChannel,
#ifndef ADC_BOOT_DRIVER
      FgAdc_DalAdcDevice_TMGetInputProperties,
      FgAdc_DalAdcDevice_TMGetRange,
      FgAdc_DalAdcDevice_TMSetThreshold,
#endif
      FgAdc_DalAdcDevice_FGSetThreshold,
      FgAdc_DalAdcDevice_FGSetBattIdForced,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalAdcDeviceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalAdcDeviceHandle.pVtbl  = &vtbl;
   pclientCtxt->DalAdcDeviceHandle.pClientCtxt = pclientCtxt;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult
FgAdc_DalAdcDevice_Attach(
   const char *pszArg,
   DALDEVICEID DeviceId,
   DalDeviceHandle **phDalDevice
   )
{
   DALResult nErr;
   FgAdcClientCtxt *pClientCtxt = NULL;

   /* Allocate memory for the client context */
   nErr = DALSYS_Malloc(sizeof(FgAdcClientCtxt), (void **)&pClientCtxt);
   if (DAL_SUCCESS != nErr || NULL == pClientCtxt)
   {
      return DAL_ERROR;
   }

   DALSYS_memset(pClientCtxt, 0, sizeof(FgAdcClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gFgAdcDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the device ID is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pFgAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gFgAdcDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pFgAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   FgAdc_InitInterface(pClientCtxt);
   (void)FgAdc_DalAdcDevice_AddRef(&(pClientCtxt->DalAdcDeviceHandle));
   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalAdcDeviceHandle);

   return DAL_SUCCESS;

error:
   DALSYS_Free(pClientCtxt);

   return nErr;
}

