/**
  @file  EFIAdc.h
  @brief ADC Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2014, 2016-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY

  when       who     what, where, why
  --------   ---     -----------------------------------------------------------
  03/13/17   jjo     Add forced batt id API.
  05/03/16   jjo     Add round robin ADC thresholds.
  04/03/14   jjo     Add additional channels.
  03/27/14   jjo     Change units of recalibration to uV.
  04/29/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
  02/13/13   jjo     Exposed USB_ID
  01/15/13   jjo     Added synchronous read API
  01/14/13   jjo     Added USB_IN and clarified IBATT polarity
  10/03/12   jjo     Added IADC channels
  07/10/12   jjo     Added string name support
  06/28/12   jjo     Updated revision
  06/15/12   jjo     Added EFI_ADC_RECALIBRATE_CHANNEL
  01/26/12   vishalo Merge in Techpubs Doxygen changes
  10/21/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
  05/12/11   jdt     Created.
=============================================================================*/
#ifndef __EFIADC_H__
#define __EFIADC_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "AdcInputs.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_adc_constants
@{ */

/** Protocol version. */
#define EFI_ADC_PROTOCOL_REVISION 0x0000000000010004

/* Protocol GUID definition */
/** @ingroup efi_adc_protocol */
#define EFI_ADC_PROTOCOL_GUID \
   { 0x61740cc4, 0x916f, 0x4471, { 0xb0, 0xdc, 0x0, 0xc6, 0xf4, 0x3a, 0x7f, 0xc6 } }

/** BATT_ID; nPhysical units are in millivolts.        */
#define EFI_ADC_INPUT_BATT_ID          ADC_INPUT_BATT_ID

/** BATT_THERM; nPhysical units are in degrees C.      */
#define EFI_ADC_INPUT_BATT_THERM       ADC_INPUT_BATT_THERM

/** VBATT; nPhysical units are in millivolts.          */
#define EFI_ADC_INPUT_VBATT            ADC_INPUT_VBATT

/** VCHG; nPhysical units are in millivolts.           */
#define EFI_ADC_INPUT_VCHG             ADC_INPUT_VCHG

/** VPH_PWR; nPhysical units are in millivolts.        */
#define EFI_ADC_INPUT_VPH_PWR          ADC_INPUT_VPH_PWR

/** PMIC_THERM; nPhysical units are in milli degreec C.     */
#define EFI_ADC_INPUT_PMIC_THERM       ADC_INPUT_PMIC_THERM

/** PMIC_HARDWARE_ID; nPhysical units are in millivolts.    */
#define EFI_ADC_INPUT_PMIC_HARDWARE_ID ADC_INPUT_PMIC_HARDWARE_ID

/** PLATFORM_ID; nPhysical units are in millivolts.    */
#define EFI_ADC_INPUT_PLATFORM_ID      ADC_INPUT_PLATFORM_ID

/** USB_IN; nPhysical units are in millivolts.         */
#define EFI_ADC_INPUT_USB_IN           ADC_INPUT_USB_IN

/** USB_ID; nPhysical units are in ohms.               */
#define EFI_ADC_INPUT_USB_ID           ADC_INPUT_USB_ID

/** IBATT_INTERNAL; nPhysical units are in microamps.
  Negative values represent current being removed from the battery.
  For recalibration, nPhysical units are in nanovolts.              */
#define EFI_ADC_INPUT_IBATT_INTERNAL   ADC_INPUT_IBATT_INTERNAL

/** IBATT_EXTERNAL; nPhysical units are in microamps.
  Negative values represent current being removed from the battery.
  For recalibration, nPhysical units are in nanovolts.              */
#define EFI_ADC_INPUT_IBATT_EXTERNAL   ADC_INPUT_IBATT_EXTERNAL

/** RSENSE_INTERNAL; nPhysical units are in nano-ohms.
  Note that only nPhysical is returned for this channel. */
#define EFI_ADC_INPUT_RSENSE_INTERNAL  ADC_INPUT_RSENSE_INTERNAL

/** RSENSE_EXTERNAL; nPhysical units are in nano-ohms.
  Note that only nPhysical is returned for this channel. */
#define EFI_ADC_INPUT_RSENSE_EXTERNAL  ADC_INPUT_RSENSE_EXTERNAL
/** @} */ /* end_addtogroup efi_adc_constants */

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/**
  External reference to the ADC Protocol GUID.
*/
extern EFI_GUID gEfiAdcProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** Protocol declaration.  */
typedef struct _EFI_ADC_PROTOCOL EFI_ADC_PROTOCOL;
/** @endcond */

/** @addtogroup efi_adc_data_types
@{ */
/** Status of an ADC result. */
typedef enum
{
   EFI_ADC_RESULT_INVALID = 0,  /**< Result is invalid (result is out of range
                                     or an error occurred during conversion). */
   EFI_ADC_RESULT_VALID,        /**< Result is valid. */
   EFI_ADC_RESULT_TIMEOUT,      /**< Result is invalid because the conversion
                                     timed out. */
   EFI_ADC_RESULT_STALE,        /**< Result is stale. */
/** @cond */
   _EFI_ADC_RESULT_STATUS_MAX_INT32 = 0x7FFFFFFF
/** @endcond */
} EfiAdcResultStatusType;

/** Data structure for the output of EFI_ADC_READ(). */
typedef struct
{
   EfiAdcResultStatusType eStatus; /**< Status of the conversion; see
                                        #EfiAdcResultStatusType for details. */
   INT32 nPhysical;      /**< Result in physical units. */
   INT32 nPercent;       /**< Result as a percentage ratio of the ADC
                             reference voltage used for the conversion
                             (if supported). 0 = 0%, 65535 = 100%. */
   INT32 nMicrovolts;    /**< Result in microvolts (if supported). */
   INT32 nCode;          /**< Raw ADC code. */
} EfiAdcResultType;

/** Data structure for the input to EFI_ADC_READ(),
  EFI_ADC_RECALIBRATE_CHANNEL(), and EFI_ADC_SYNCHRONOUS_READ(). This
  structure is obtained by calling EFI_ADC_GET_CHANNEL(). */
typedef struct
{
   UINT32 nDeviceIdx;   /**< Device index. */
   UINT32 nChannelIdx;  /**< Channel index. */
} EfiAdcDeviceChannelType;

/** Data structure for the output of EFI_ADC_RECALIBRATE_CHANNEL(). */
typedef struct
{
   EfiAdcResultStatusType eStatus; /**< Status of the recalibration; see
                                        #EfiAdcResultStatusType for details. */
   INT32 nPhysical1_uV;     /**< Physical value for reference 1. */
   INT32 nPhysical2_uV;     /**< Physical value for reference 2. */
   UINT32 uCode1;        /**< Raw ADC code for reference 1. */
   UINT32 uCode2;        /**< Raw ADC code for reference 2. @newpagetable */
} EfiAdcRecalibrateChannelResultType;

/** Hardware thresholds supported by the fuel gauge ADC. Physical units for
  thresholds are in degrees C. */
typedef enum
{
   EFI_ADC_THRESH_SKIN_HOT,        /**< Skin temperature hot threshold. */
   EFI_ADC_THRESH_SKIN_TOO_HOT,    /**< Skin temperature too hot threshold. */
   EFI_ADC_THRESH_CHARGER_HOT,     /**< Charger temperature hot threshold. */
   EFI_ADC_THRESH_CHARGER_TOO_HOT  /**< Charger temperature too hot threshold. */
} EfiAdcFuelGaugeThresholdType;
/** @} */ /* end_addtogroup efi_adc_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ADC_GET_CHANNEL */
/** @ingroup efi_adc_get_channel
  @par Summary
  Gets an ADC channel using a logical ADC input string ID.

  @param[in]  pszInputName    NULL-terminated ADC input ID string.
  @param[in]  nInputNameSize  Size of the string (including the NULL-terminator).
  @param[out] pChannel        Device channel for the ADC input;
                              see #EfiAdcDeviceChannelType for details.

  @par Description
  This function looks up the device-channel index using the string
  name passed in by the client.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Channel is not present.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_GET_CHANNEL)(
  IN CONST CHAR8 *pszInputName,
  IN UINT32 nInputNameSize,
  OUT EfiAdcDeviceChannelType *pChannel
);

/* EFI_ADC_READ */
/** @ingroup efi_adc_read
  @par Summary
  Reads an ADC channel.

  @param[in]   pEfiChannel    ADC channel to read instance;
                              see #EfiAdcDeviceChannelType for details.
  @param[out]  pEfiAdcResult  Result of the ADC reading;
                              see #EfiAdcResultType for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_READ)(
  IN EfiAdcDeviceChannelType *pEfiChannel,
  OUT EfiAdcResultType *pEfiAdcResult
);

/* EFI_ADC_RECALIBRATE_CHANNEL */
/** @ingroup efi_adc_recalibrate_channel
  @par Summary
  Recalibrates an ADC channel.

  @param[in]   pEfiChannel    ADC channel to recalibrate instance;
                              see #EfiAdcDeviceChannelType for details.
  @param[out]  pEfiAdcRecalibrateChannelResult  Result of the recalibration;
                              see #EfiAdcRecalibrateChannelResultType for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_RECALIBRATE_CHANNEL)(
  IN EfiAdcDeviceChannelType *pEfiChannel,
  OUT EfiAdcRecalibrateChannelResultType *pEfiAdcRecalibrateChannelResult
);

/* EFI_ADC_SYNCHRONOUS_READ */
/** @ingroup efi_adc_synchronous_read
  @par Summary
  Performs a synchronous VADC and IADC measurement.

  @param[in]   pVAdcChannel    Voltage ADC channel to read instance;
                               see #EfiAdcDeviceChannelType for details.
  @param[in]   pIAdcChannel    Current ADC channel to read instance;
                               see #EfiAdcDeviceChannelType for details.
  @param[out]  pVAdcResult     Voltage ADC read result;
                               see #EfiAdcResultType for details.
  @param[out]  pIAdcResult     Current ADC read result;
                               see #EfiAdcResultType for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_SYNCHRONOUS_READ)(
  IN EfiAdcDeviceChannelType *pVAdcChannel,
  IN EfiAdcDeviceChannelType *pIAdcChannel,
  OUT EfiAdcResultType *pVAdcResult,
  OUT EfiAdcResultType *pIAdcResult
);

/* EFI_ADC_SET_FUEL_GAUGE_THRESHOLD */
/** @ingroup efi_adc_set_fuel_gauge_threshold
  @par Summary
  Sets a fuel gauge hardware threshold.

  @param[in]   eWhichThresh   Which threshold to set;
                              see #EfiAdcFuelGaugeThresholdType for details.
  @param[in]   nThreshold     The threshold value in physical units.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_SET_FUEL_GAUGE_THRESHOLD)(
  IN EfiAdcFuelGaugeThresholdType eWhichThresh,
  IN INT32 nThreshold
);

/* EFI_ADC_SET_FUEL_GAUGE_BATT_ID_FORCED */
/** @ingroup efi_adc_set_fuel_gauge_batt_id_forced
  @par Summary
  Sets battery ID readings to forced mode.

  @param[in]   bEnable        Enabled or disabled the forced mode.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADC_SET_FUEL_GAUGE_BATT_ID_FORCED)(
   IN BOOLEAN bEnable
);


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_adc_protocol
  @par Summary
  ADC Protocol interface.

  @par Parameters
  @inputprotoparams{adc_proto_params.tex}
*/
struct _EFI_ADC_PROTOCOL {
  UINT64                       Revision;
  EFI_ADC_GET_CHANNEL          GetChannel;
  EFI_ADC_READ                 AdcRead;
  EFI_ADC_RECALIBRATE_CHANNEL  AdcRecalibrateChannel;
  EFI_ADC_SYNCHRONOUS_READ     AdcSynchronousRead;
  EFI_ADC_SET_FUEL_GAUGE_THRESHOLD  AdcSetFuelGaugeThreshold;
  EFI_ADC_SET_FUEL_GAUGE_BATT_ID_FORCED AdcSetFuelGaugeBattIdForced;
};

#endif /* __EFIADC_H__ */

