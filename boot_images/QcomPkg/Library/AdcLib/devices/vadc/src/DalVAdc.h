#ifndef __DALVADC_H__
#define __DALVADC_H__
/*============================================================================
  @file DalVAdc.h

  Function and data structure declarations for VADC DAL


                Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/DalVAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DDIAdcDevice.h"
#include "VAdcBsp.h"
#include "VAdcHal.h"
#include "VAdcLog.h"
#include "VAdcCommon.h"
#include "AdcScalingUtil.h"
#include "AdcSpmi.h"
#include "pm_version.h"
#include "pm_gpio.h"
#ifndef ADC_BOOT_DRIVER
#include "npa.h"
#endif

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
enum
{
   VADC_EVENT_DEFAULT = 0,
   VADC_EVENT_TIMEOUT,
   _VADC_NUM_EVENTS
};

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   VADC_AVERAGED_CHANNEL_STATE_UNKNOWN = 0,
   VADC_AVERAGED_CHANNEL_STATE_LOW,
   VADC_AVERAGED_CHANNEL_STATE_LOW_MED,
   VADC_AVERAGED_CHANNEL_STATE_MED,
   VADC_AVERAGED_CHANNEL_STATE_MED_HIGH,
   VADC_AVERAGED_CHANNEL_STATE_HIGH
} VAdcAveragedChannelStateType;

typedef struct
{
   uint32 uLowChannel;
   uint32 uMedChannel;
   uint32 uHighChannel;
   VAdcAveragedChannelStateType eState;
} VAdcAveragedChannelType;

/*
 * Declaring a "VAdc" Driver, device and client context
 */
typedef struct VAdcDrvCtxt VAdcDrvCtxt;
typedef struct VAdcDevCtxt VAdcDevCtxt;
typedef struct VAdcClientCtxt VAdcClientCtxt;

/*
 * Declaring a private "VAdc" Vtable
 */
typedef struct VAdcDALVtbl VAdcDALVtbl;
struct VAdcDALVtbl
{
  int (*VAdc_DriverInit)(VAdcDrvCtxt *);
  int (*VAdc_DriverDeInit)(VAdcDrvCtxt *);
};

struct VAdcDevCtxt
{
   // Base Members
   uint32 dwRefs;
   DALDEVICEID DevId;
   uint32 dwDevCtxtRefIdx;
   VAdcDrvCtxt *pVAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   // VAdc Dev state can be added by developers here
   VAdcDebugInfoType debug;                         /* debug structure */
   AdcSpmiCtxtType spmiCtxt;                        /* SPMI wrapper context */
   VAdcHalInterfaceType iVAdcHalInterface;          /* VADC HAL interface */
   VAdcRevisionInfoType revisionInfo;               /* VADC revision info */
   pm_device_info_type pmicDeviceInfo;              /* PMIC device info */
   const VAdcBspType *pBsp;                         /* pointer to the BSP */
   VAdcAveragedChannelType *paAveragedChannels;     /* config for averaged channels */
#ifndef ADC_BOOT_DRIVER
   DALSYSEventHandle ahEocEvent[_VADC_NUM_EVENTS];  /* end-of-conversion event - EOC signal & a timeout */
   npa_client_handle hNPACpuLatency;                /* npa handle for min latency vote */
#endif
   uint32 uDeviceIndex;                             /* device index */
   uint32 uTotalNumChannels;                        /* total number of channels */
   boolean bHardwareSupported;                      /* flag to indicate if the hardware is supported */
};

struct VAdcDrvCtxt
{
   // Base Members
   VAdcDALVtbl VAdcDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   VAdcDevCtxt VAdcDevCtxt[VADC_MAX_NUM_DEVICES];
   // VAdc Drv state can be added by developers here
};

/*
 * Declaring a "VAdc" Client Context
 */
struct VAdcClientCtxt
{
   // Base Members
   uint32 dwRefs;
   uint32 dwAccessMode;
   void *pPortCtxt;
   VAdcDevCtxt *pVAdcDevCtxt;
   DalAdcDeviceHandle DalAdcDeviceHandle;
   // VAdc Client state can be added by developers here
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* Functions specific to DAL */
DALResult VAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);
DALResult VAdc_DriverInit(VAdcDrvCtxt *);
DALResult VAdc_DriverDeInit(VAdcDrvCtxt *);
DALResult VAdc_DeviceInit(VAdcClientCtxt *);
DALResult VAdc_DeviceDeInit(VAdcClientCtxt *);
DALResult VAdc_Open(VAdcClientCtxt *, uint32);
DALResult VAdc_Close(VAdcClientCtxt *);

/* Functions specific to DalAdcDevice interface */
DALResult VAdc_SetDeviceIndex(VAdcClientCtxt *, uint32);
DALResult VAdc_GetDeviceProperties(VAdcClientCtxt *, AdcDeviceDevicePropertiesType *);
DALResult VAdc_GetChannel(VAdcClientCtxt *, const char *, uint32 *);
DALResult VAdc_ReadChannel(VAdcClientCtxt *, uint32, AdcDeviceResultType *);
DALResult VAdc_RecalibrateChannel(VAdcClientCtxt *, uint32, AdcDeviceRecalibrateResultType *);

#endif /* #ifndef __DALVADC_H__ */

