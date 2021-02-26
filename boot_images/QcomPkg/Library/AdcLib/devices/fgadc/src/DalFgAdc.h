#ifndef __DALFGADC_H__
#define __DALFGADC_H__
/*============================================================================
  @file DalFgAdc.h

  Function and data structure declarations for FGADC DAL


                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/DalFgAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DDIAdcDevice.h"
#include "FgAdcBsp.h"
#include "FgAdcHal.h"
#include "FgAdcLog.h"
#include "AdcScalingUtil.h"
#include "AdcSpmi.h"
#include "pm_version.h"
#include "pm_gpio.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/*
 * Declaring a "FgAdc" Driver, device and client context
 */
typedef struct FgAdcDrvCtxt FgAdcDrvCtxt;
typedef struct FgAdcDevCtxt FgAdcDevCtxt;
typedef struct FgAdcClientCtxt FgAdcClientCtxt;

/*
 * Declaring a private "FgAdc" Vtable
 */
typedef struct FgAdcDALVtbl FgAdcDALVtbl;
struct FgAdcDALVtbl
{
  int (*FgAdc_DriverInit)(FgAdcDrvCtxt *);
  int (*FgAdc_DriverDeInit)(FgAdcDrvCtxt *);
};

struct FgAdcDevCtxt
{
   // Base Members
   uint32 dwRefs;
   DALDEVICEID DevId;
   uint32 dwDevCtxtRefIdx;
   FgAdcDrvCtxt *pFgAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   // FgAdc Dev state can be added by developers here
   FgAdcDebugInfoType debug;                        /* debug structure */
   AdcSpmiCtxtType spmiCtxt;                        /* SPMI wrapper context */
   FgAdcHalInterfaceType iFgAdcHalInterface;        /* FGADC HAL interface */
   FgAdcRevisionInfoType revisionInfo;              /* FGADC revision info */
   pm_device_info_type pmicDeviceInfo;              /* PMIC device info */
   const FgAdcBspType *pBsp;                        /* pointer to the BSP */
   uint32 uDeviceIndex;                             /* device index */
   uint32 uChargerTempIdx;                          /* channel index for charger temp */
   uint32 uFabID;                                   /* fab ID */
   boolean bHardwareSupported;                      /* flag to indicate if the hardware is supported */
};

struct FgAdcDrvCtxt
{
   // Base Members
   FgAdcDALVtbl FgAdcDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   FgAdcDevCtxt FgAdcDevCtxt[FGADC_MAX_NUM_DEVICES];
   // FgAdc Drv state can be added by developers here
};

/*
 * Declaring a "FgAdc" Client Context
 */
struct FgAdcClientCtxt
{
   // Base Members
   uint32 dwRefs;
   uint32 dwAccessMode;
   void *pPortCtxt;
   FgAdcDevCtxt *pFgAdcDevCtxt;
   DalAdcDeviceHandle DalAdcDeviceHandle;
   // FgAdc Client state can be added by developers here
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* Functions specific to DAL */
DALResult FgAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
DALResult FgAdc_DriverInit(FgAdcDrvCtxt *);
DALResult FgAdc_DriverDeInit(FgAdcDrvCtxt *);
DALResult FgAdc_DeviceInit(FgAdcClientCtxt *);
DALResult FgAdc_DeviceDeInit(FgAdcClientCtxt *);
DALResult FgAdc_Open(FgAdcClientCtxt *, uint32);
DALResult FgAdc_Close(FgAdcClientCtxt *);

/* Functions specific to DalAdcDevice interface */
DALResult FgAdc_SetDeviceIndex(FgAdcClientCtxt *, uint32);
DALResult FgAdc_GetDeviceProperties(FgAdcClientCtxt *, AdcDeviceDevicePropertiesType *);
DALResult FgAdc_GetChannel(FgAdcClientCtxt *, const char *, uint32 *);
DALResult FgAdc_ReadChannel(FgAdcClientCtxt *, uint32, AdcDeviceResultType *);
DALResult FgAdc_SetThreshold(FgAdcClientCtxt *, AdcDeviceFGThresholdType, int32);
DALResult FgAdc_SetBattIdForced(FgAdcClientCtxt *, DALBOOL);

#endif /* #ifndef __DALFGADC_H__ */

