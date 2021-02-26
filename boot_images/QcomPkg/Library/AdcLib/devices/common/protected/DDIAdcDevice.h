#ifndef __DDIADCDEVICE_H__
#define __DDIADCDEVICE_H__
/*============================================================================
  @file DDIAdcDevice.h

  ADC Physical Device Driver Interface header

  This header is to be included solely by the ADC DAL and implementations of
  this DDI. It provides a common interface for the ADC DAL to interface with
  different ADC hardware.

  External clients may not access these interfaces.

                Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/common/protected/DDIAdcDevice.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DalDevice.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DALADCDEVICE_INTERFACE_VERSION DALINTERFACE_VERSION(4,4)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
enum
{
   ADC_DEVICE_ERROR_OUT_OF_TM_CLIENTS = 1,
   ADC_DEVICE_ERROR_TM_NOT_SUPPORTED,
   ADC_DEVICE_ERROR_TM_THRESHOLD_OUT_OF_RANGE,
   ADC_DEVICE_ERROR_TM_INVALID_MEAS_IDX,
   ADC_DEVICE_ERROR_TM_BUSY,
   ADC_DEVICE_ERROR_FG_NOT_SUPPORTED
};

typedef enum
{
   ADC_DEVICE_CHANNEL_READY,
   ADC_DEVICE_CHANNEL_BUSY,
   ADC_DEVICE_CHANNEL_CALIBRATING,
   ADC_DEVICE_CHANNEL_ERROR,
   _ADC_MAX_DEVICE_CHANNEL_STATUS = 0x7FFFFFFF
} AdcDeviceChannelStatusType;

typedef enum
{
   ADC_DEVICE_RESULT_VALID,
   ADC_DEVICE_RESULT_INVALID,
   ADC_DEVICE_RESULT_INVALID_CHANNEL,
   ADC_DEVICE_RESULT_TIMEOUT,
   ADC_DEVICE_RESULT_STALE,
   _ADC_MAX_DEVICE_RESULT_STATUS = 0x7FFFFFFF
} AdcDeviceResultStatusType;

typedef struct
{
   uint32 uNumChannels;      /* number of ADC channels */
   uint32 uNumMeas;          /* number of TM measurements */
} AdcDeviceDevicePropertiesType;

typedef struct
{
   AdcDeviceResultStatusType eStatus;
   int32 nPhysical1_uV;    /* ref 1 in microvolts */
   int32 nPhysical2_uV;    /* ref 2 in microvolts */
   uint32 uCode1;          /* raw ADC code for ref 1 */
   uint32 uCode2;          /* raw ADC code for ref 2 */
} AdcDeviceRecalibrateResultType;

typedef struct
{
   AdcDeviceResultStatusType eStatus;
   int32 nPhysical;     /* result in physical units. Units depend on BSP */
   uint32 uPercent;     /* result as percentage of reference voltage used
                         * for conversion. 0 = 0%, 65535 = 100% */
   uint32 uMicrovolts;  /* result in microvolts */
   uint32 uCode;        /* raw ADC code */
} AdcDeviceResultType;

typedef struct
{
   int32 nPhysicalMin;   /* Minimum threshold in physical units */
   int32 nPhysicalMax;   /* Maximum threshold in physical units */
} AdcDeviceTMRangeType;

typedef enum
{
   ADC_DEVICE_TM_THRESHOLD_LOWER,    /* Lower threshold */
   ADC_DEVICE_TM_THRESHOLD_HIGHER,   /* Higher threshold */
   ADC_DEVICE_TM_NUM_THRESHOLDS
} AdcDeviceTMThresholdType;

typedef enum
{
   ADC_DEVICE_FG_THRESH_SKIN_HOT,        /* Skin temperature hot threshold */
   ADC_DEVICE_FG_THRESH_SKIN_TOO_HOT,    /* Skin temperature too hot threshold */
   ADC_DEVICE_FG_THRESH_CHARGER_HOT,     /* Charger temperature hot threshold */
   ADC_DEVICE_FG_THRESH_CHARGER_TOO_HOT, /* Charger temperature too hot threshold */
} AdcDeviceFGThresholdType;

typedef struct DalAdcDevice DalAdcDevice;
struct DalAdcDevice
{
   DalDevice DalDevice;
   DALResult (*SetDeviceIndex)(DalDeviceHandle * _h, uint32);
   DALResult (*GetInputProperties)(DalDeviceHandle * _h, const char * pChannelName, uint32 *);
   DALResult (*GetDeviceProperties)(DalDeviceHandle * _h, AdcDeviceDevicePropertiesType *pAdcDeviceProp);
#ifndef ADC_LOADER_DRIVER
   DALResult (*RecalibrateChannel)(DalDeviceHandle * _h, uint32  uChannelIdx, AdcDeviceRecalibrateResultType *);
#endif
   DALResult (*ReadChannel)(DalDeviceHandle * _h, uint32 uChannelIdx, AdcDeviceResultType *);
#ifndef ADC_BOOT_DRIVER
   DALResult (*TMGetInputProperties)(DalDeviceHandle * _h, const char * pInputName, uint32 *puChannelIdx);
   DALResult (*TMGetRange)(DalDeviceHandle *h, uint32 uMeasIdx, AdcDeviceTMRangeType *pAdcTMDeviceRange);
   DALResult (*TMSetThreshold)(DalDeviceHandle *_h, uint32 uChannelId, const DALSYSEventHandle hEvent, AdcDeviceTMThresholdType eThreshold, const int32 *pnThresholdDesired, int32 *pnThresholdSet);
#endif
   DALResult (*FGSetThreshold)(DalDeviceHandle *_h, AdcDeviceFGThresholdType eWhichThresh, int32 nThreshold);
   DALResult (*FGSetBattIdForced)(DalDeviceHandle *_h, DALBOOL bEnable);
};

typedef struct DalAdcDeviceHandle DalAdcDeviceHandle;
struct DalAdcDeviceHandle
{
   uint32 dwDalHandleId;
   const DalAdcDevice *pVtbl;
   void *pClientCtxt;
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   @brief Attaches to the ADC physical device driver

   This function attaches the client to the physical ADC device driver.
   This function is used to obtain the device handle which is required to
   use the driver APIs.

   @param  pszDevName [in] The device ID string to attach to
   @param  hDalDevice [out] Pointer to DAL device handle pointer which will receive a pointer to the ADC device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.

*/
#define DAL_AdcDeviceDeviceAttach(pszDevName,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL, pszDevName, DALADCDEVICE_INTERFACE_VERSION, hDalDevice)

/**
   @brief Sets the device index

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uDeviceIndex [in] Device index

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_SetDeviceIndex(DalDeviceHandle * _h, uint32 uDeviceIndex)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->SetDeviceIndex( _h, uDeviceIndex);
}

/**
   @brief Gets the number of channels defined for this device

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  pAdcDeviceProp [out] Pointer to result data

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_GetDeviceProperties(DalDeviceHandle * _h, AdcDeviceDevicePropertiesType *pAdcDeviceProp)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetDeviceProperties( _h, pAdcDeviceProp);
}

/**
   Determine whether the channel is supported by the device.

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  pChannelName [in] NULL-terminated channel name
   @param  puChannelIdx [out] Channel index

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_GetInputProperties(DalDeviceHandle * _h, const char *pChannelName, uint32 *puChannelIdx)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->GetInputProperties( _h, pChannelName, puChannelIdx);
}

#ifndef ADC_LOADER_DRIVER
/**
   @brief Recalibrates a specific channel

   This function is used to calibrate a specific channel. If multiple channels
   use the same configuration, then those channels will also be recalibrated.

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx [in] The index of the channel to recalibrate
   @param  pAdcDeviceRecalibrateResult [out] Recalibration result

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_RecalibrateChannel(DalDeviceHandle *_h, uint32 uChannelIdx, AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->RecalibrateChannel( _h, uChannelIdx, pAdcDeviceRecalibrateResult);
}
#endif

/**
   @brief Reads an ADC channel

   This function is used to read an ADC channel.

   @param  _h [in] Device handle obtained from DAL_AdcDeviceDeviceAttach
   @param  uChannelIdx [in] The index of the channel to recalibrate
   @param  pAdcDeviceReadResult [out] Read result

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_ReadChannel(DalDeviceHandle *_h, uint32 uChannelIdx, AdcDeviceResultType *pAdcDeviceReadResult)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->ReadChannel( _h, uChannelIdx, pAdcDeviceReadResult);
}

#ifndef ADC_BOOT_DRIVER
/**
   @brief Gets the properties for the TM ADC

   This function looks up the properties for an analog input.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceDeviceAttach
   @param  pInputName [in] The name of the analog input to look up
   @param  puMeasIdx [out] The measurement index

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_TMGetInputProperties(DalDeviceHandle * _h, const char *pInputName, uint32 *puMeasIdx)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->TMGetInputProperties( _h, pInputName, puMeasIdx);
}

/**
   @brief Gets the range of physical values that can be set for a given TM channel.

   This function gets the minimum and maximum physical value that can be set as a
   threshold for a given VADC TM channel.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceDeviceAttach
   @param  uMeasIdx [in] The VADC TM channel obtained by calling DalAdcDevice_TMGetInputProperties
   @param  pAdcTMDeviceRange [out] Structure with minimum and maximum physical values

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_TMGetRange(DalDeviceHandle * _h, uint32 uMeasIdx, AdcDeviceTMRangeType *pAdcTMDeviceRange)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->TMGetRange( _h, uMeasIdx, pAdcTMDeviceRange);
}

/**
   @brief Sets a threshold with the ADC TM

   The threshold event will be triggered once when the threshold is crossed.
   After the event is triggered, the threshold will not trigger the event again
   and will be in a triggered state until the client calls DalAdcDevice_TMSetThreshold
   to set a new threshold.

   Note that thresholds can be disabled/re-enabled on a per client basis by calling
   DalAdcDevice_TMSetEnableThresholds. Thresholds are enabled by default, but calling
   DalAdcDevice_TMSetThreshold does not automatically re-enable them if they were previously
   disabled by a call to DalAdcDevice_TMSetEnableThresholds.

   @param  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceDeviceAttach
   @param  uMeasIdx [in] measurement index from DalAdc_TMGetInputProperties
   @param  hEvent [in] Threshold event that gets set
   @param  eThreshold [in] Type of threshold to set
   @param  pnThresholdDesired [in] Threshold value to set in physical units; If NULL the
                                current threshold is cleared.
   @param  pnThresholdSet [out] The actual threshold value that was set in physical units.


   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS if successful.

*/
static __inline DALResult
DalAdcDevice_TMSetThreshold(DalDeviceHandle *_h, uint32 uMeasIdx, const DALSYSEventHandle hEvent, AdcDeviceTMThresholdType eThreshold, const int32 *pnThresholdDesired, int32 *pnThresholdSet)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->TMSetThreshold( _h, uMeasIdx, hEvent, eThreshold, pnThresholdDesired, pnThresholdSet);
}
#endif

/**
   @brief Sets a fuel gauge hardware threshold.

   This API allows clients to specify a fuel gauge hardware threshold. The threshold's
   units are physical units of degrees Celsius.

   @param[in]  _h [in] Handle to the ADC driver instance instantiated by DAL_AdcDeviceDeviceAttach
   @param[in]  eWhichThresh Which threshold to set
   @param[in]  nThreshold The threshold value in physical units

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS on success. Other return values indicate an error.

*/
static __inline DALResult
DalAdcDevice_FGSetThreshold(DalDeviceHandle *_h, AdcDeviceFGThresholdType eWhichThresh, int32 nThreshold)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->FGSetThreshold( _h, eWhichThresh, nThreshold);
}

/**
   @brief Sets batt ID trigger forced on

   Forces battery ID readings all the time and puts RR ADC in continuous mode.

   @param[in]  _h Handle to the ADC driver instance instantiated by DAL_AdcDeviceDeviceAttach
   @param[in]  bEnable Enable / disable this mode

   @see    DAL_AdcDeviceDeviceAttach

   @return DAL_SUCCESS on success. Other return values indicate an error.

*/
static __inline DALResult
DalAdcDevice_FGSetBattIdForced(DalDeviceHandle *_h, DALBOOL bEnable)
{
   return ((DalAdcDeviceHandle *)_h)->pVtbl->FGSetBattIdForced( _h, bEnable);
}

#endif

