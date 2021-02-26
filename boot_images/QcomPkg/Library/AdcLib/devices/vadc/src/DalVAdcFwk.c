/*============================================================================
  FILE:         DalVAdcFwk.c

  OVERVIEW:     VADC DAL framework file.

  DEPENDENCIES: None

                Copyright (c) 2009-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/DalVAdcFwk.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-03-13  jjo  Forced batt id workaround.
  2016-05-20  jjo  FG threshold API.
  2015-03-27  jjo  Added tolerance API.
  2015-01-12  jjo  Add TM APIs.
  2014-01-15  jjo  KW fix to free memory on error.
  2013-06-19  jjo  Added support for multiple VADCs.
  2012-17-10  jjo  Added synchronous call stubs.
  2012-06-13  jjo  Updated to use a DAL device ID string name.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdcDevice.h"
#include "DalVAdc.h"
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
VAdcDrvCtxt gVAdcDrvCtxt =
{
   .VAdcDALVtbl   = {VAdc_DriverInit, VAdc_DriverDeInit},
   .dwNumDev      = VADC_MAX_NUM_DEVICES,
   .dwSizeDevCtxt = sizeof(VAdcDevCtxt),
   .bInit         = 0,
   .dwRefs        = 0,
   .VAdcDevCtxt   = {{0}}
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32
VAdc_DalAdcDevice_AddRef(DalAdcDeviceHandle *h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32
VAdc_DalAdcDevice_Detach(DalDeviceHandle *h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
VAdc_DalAdcDevice_Init(DalDeviceHandle *h)
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_DeviceInit(pCtxt);
}

static DALResult
VAdc_DalAdcDevice_DeInit(DalDeviceHandle *h)
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_DeviceDeInit(pCtxt);
}

static DALResult
VAdc_DalAdcDevice_PowerEvent(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain
   )
{
   return DAL_SUCCESS;
}

static DALResult
VAdc_DalAdcDevice_Open(
   DalDeviceHandle *h,
   uint32 mode
   )
{
   return VAdc_Open(h->pClientCtxt, mode);
}

static DALResult
VAdc_DalAdcDevice_Close(DalDeviceHandle *h)
{
   return VAdc_Close(h->pClientCtxt);
}

static DALResult
VAdc_DalAdcDevice_Info(
   DalDeviceHandle *h,
   DalDeviceInfo* info,
   uint32 infoSize
   )
{
   info->Version = DALADCDEVICE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
VAdc_DalAdcDevice_SysRequest(
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
VAdc_DalAdcDevice_SetDeviceIndex(
   DalDeviceHandle *h,
   uint32 uDeviceIndex
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_SetDeviceIndex(pCtxt,
                              uDeviceIndex);
}

static DALResult
VAdc_DalAdcDevice_GetInputProperties(
   DalDeviceHandle *h,
   const char *pChannelName,
   uint32 *puChannelIdx
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_GetChannel(pCtxt,
                          pChannelName,
                          puChannelIdx);
}

static DALResult
VAdc_DalAdcDevice_GetDeviceProperties(
   DalDeviceHandle *h,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_GetDeviceProperties(pCtxt, pAdcDeviceProp);
}

#ifndef ADC_LOADER_DRIVER
static DALResult
VAdc_DalAdcDevice_RecalibrateChannel(
   DalDeviceHandle *h,
   uint32 uChannelIdx,
   AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_RecalibrateChannel(pCtxt,
                                  uChannelIdx,
                                  pAdcDeviceRecalibrateResult);
}
#endif

static DALResult
VAdc_DalAdcDevice_ReadChannel(
   DalDeviceHandle *h,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceReadResult
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_ReadChannel(pCtxt,
                           uChannelIdx,
                           pAdcDeviceReadResult);
}

#ifndef ADC_BOOT_DRIVER
static DALResult
VAdc_DalAdcDevice_TMGetInputProperties(
   DalDeviceHandle*h,
   const char *pInputName,
   uint32 *puMeasIdx
   )
{
   return ADC_DEVICE_ERROR_TM_NOT_SUPPORTED;
}

static DALResult
VAdc_DalAdcDevice_TMGetRange(
   DalDeviceHandle *h,
   uint32 uMeasIdx,
   AdcDeviceTMRangeType *pAdcDeviceTMRange
   )
{
   return ADC_DEVICE_ERROR_TM_NOT_SUPPORTED;
}

static DALResult
VAdc_DalAdcDevice_TMSetThreshold(
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
VAdc_DalAdcDevice_FGSetThreshold(
   DalDeviceHandle *h,
   AdcDeviceFGThresholdType eWhichThresh,
   int32 nThreshold
   )
{
   return ADC_DEVICE_ERROR_FG_NOT_SUPPORTED;
}

static DALResult
VAdc_DalAdcDevice_FGSetBattIdForced(
   DalDeviceHandle *h,
   DALBOOL bEnable
   )
{
   return ADC_DEVICE_ERROR_FG_NOT_SUPPORTED;
}

static void
VAdc_InitInterface(VAdcClientCtxt *pclientCtxt)
{
   static const DalAdcDevice vtbl =
   {
      {
         VAdc_DalAdcDevice_Attach,
         VAdc_DalAdcDevice_Detach,
         VAdc_DalAdcDevice_Init,
         VAdc_DalAdcDevice_DeInit,
         VAdc_DalAdcDevice_Open,
         VAdc_DalAdcDevice_Close,
         VAdc_DalAdcDevice_Info,
         VAdc_DalAdcDevice_PowerEvent,
         VAdc_DalAdcDevice_SysRequest
      },
      VAdc_DalAdcDevice_SetDeviceIndex,
      VAdc_DalAdcDevice_GetInputProperties,
      VAdc_DalAdcDevice_GetDeviceProperties,
#ifndef ADC_LOADER_DRIVER
      VAdc_DalAdcDevice_RecalibrateChannel,
#endif
      VAdc_DalAdcDevice_ReadChannel,
#ifndef ADC_BOOT_DRIVER
      VAdc_DalAdcDevice_TMGetInputProperties,
      VAdc_DalAdcDevice_TMGetRange,
      VAdc_DalAdcDevice_TMSetThreshold,
#endif
      VAdc_DalAdcDevice_FGSetThreshold,
      VAdc_DalAdcDevice_FGSetBattIdForced,
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
VAdc_DalAdcDevice_Attach(
   const char *pszArg,
   DALDEVICEID DeviceId,
   DalDeviceHandle **phDalDevice
   )
{
   DALResult nErr;
   VAdcClientCtxt *pClientCtxt = NULL;

   /* Allocate memory for the client context */
   nErr = DALSYS_Malloc(sizeof(VAdcClientCtxt), (void **)&pClientCtxt);
   if (DAL_SUCCESS != nErr || NULL == pClientCtxt)
   {
      return DAL_ERROR;
   }

   DALSYS_memset(pClientCtxt, 0, sizeof(VAdcClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gVAdcDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the device ID is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gVAdcDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   VAdc_InitInterface(pClientCtxt);
   (void)VAdc_DalAdcDevice_AddRef(&(pClientCtxt->DalAdcDeviceHandle));
   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalAdcDeviceHandle);

   return DAL_SUCCESS;

error:
   DALSYS_Free(pClientCtxt);

   return nErr;
}

