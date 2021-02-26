/** 
@file  EFIPmicRTC.h
@brief PMIC RTC Protocol for UEFI.
*/
/*=============================================================================
Copyright (c) 2011 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
05/21/15   al      Adding API to read RTC alarm expired and enabled status
06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup
01/22/13   al	   Added SetTimeValueRaw
10/23/12   al	   Updating copyright info
09/12/12   al      Porting for badger
7/27/12    al	   Renamed all Enums,added device index and resource index
01/26/12   vishalo Merge in Techpubs Doxygen change
10/25/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
03/20/11   dy      created

=============================================================================*/
#ifndef __EFIPMICRTC_H__
#define __EFIPMICRTC_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicRtc_constants 
@{ */
/**
Protocol version.
*/
#define PMIC_RTC_REVISION 0x0000000000010003
/** @} */ /* end_addtogroup efi_pmicRtc_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicRtc_protocol */
#define EFI_PMICRTC_PROTOCOL_GUID \
	{ 0xfcbedc3e, 0xd2a6, 0x4234, { 0x9b, 0xe5, 0xc5, 0xea, 0x93, 0xd1, 0xf3, 0x92 } }

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC RTC Protocol GUID.
*/
extern EFI_GUID gQcomPmicRtcProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_RTC_PROTOCOL  EFI_QCOM_PMIC_RTC_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicRtc_data_types 
@{ */
/**
RTC display.
*/
typedef enum
{
	EFI_PM_RTC_12HR_MODE,   /**< 24-hr mode. */
	EFI_PM_RTC_24HR_MODE,   /**< 12-hr mode. */
	EFI_PM_RTC_INVALID_MODE /**< Invalid mode. */
} EFI_PM_RTC_DISPLAY_TYPE;

/**
PMIC version of the Julian time structure.
*/
typedef struct
{
	UINT16 year;        /**< Year. Range: 1980 to 2100. */
	UINT16 month;       /**< Month. Range: 1 to 12. */
	UINT16 day;         /**< Day. Range: 1 to 31. */
	UINT16 hour;        /**< Hour. Range: 0 to 23. */
	UINT16 minute;      /**< Minute. Range: 0 to 59. */
	UINT16 second;      /**< Second. Range: 0 to 59. */
	UINT16 day_of_week; /**< Day of the week. Range: 0 to 6; Monday to Sunday. */
} EFI_PM_RTC_JULIAN_TYPE;
/** @} */ /* end_addtogroup efi_pmicRtc_data_types */

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_RTC_GET_DISPLAY_MODE */ 
/** @ingroup efi_pmicRtc_get_display_mode
@par Summary
Gets the RTC display mode.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[out] ModePtr          Display mode; see #EFI_PM_RTC_DISPLAY_TYPE 
for details.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_GET_DISPLAY_MODE)(
	IN UINT32 PmicDeviceIndex, 
	OUT EFI_PM_RTC_DISPLAY_TYPE *ModePtr
	);


/* EFI_PM_RTC_SET_DISPLAY_MODE */ 
/** @ingroup efi_pmicRtc_set_display_mode
@par Summary
Sets the RTC display mode.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  Mode             Display mode; see #EFI_PM_RTC_DISPLAY_TYPE 
for details.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_SET_DISPLAY_MODE)(
	IN UINT32 PmicDeviceIndex, 
	IN EFI_PM_RTC_DISPLAY_TYPE Mode
	);


/* EFI_PM_RTC_GET_TIME_VALUE */ 
/** @ingroup efi_pmicRtc_get_time_value
@par Summary
Gets the RTC time value in Julian format.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[out] CurrentTimePtr   Time in Julian format; see 
#EFI_PM_RTC_JULIAN_TYPE for details.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_GET_TIME_VALUE)(
	IN UINT32 PmicDeviceIndex, 
	OUT EFI_PM_RTC_JULIAN_TYPE *CurrentTimePtr
	);


/* EFI_PM_RTC_SET_TIME_VALUE */ 
/** @ingroup efi_pmicRtc_set_time_value
@par Summary
Sets the RTC time value in Julian format.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[out] CurrentTimePtr   Time in Julian format; see 
#EFI_PM_RTC_JULIAN_TYPE for details.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_SET_TIME_VALUE)(
	IN UINT32 PmicDeviceIndex, 
	OUT EFI_PM_RTC_JULIAN_TYPE *CurrentTimePtr
	);


/* EFI_PM_RTC_IS_TIME_VALID */ 
/** @ingroup efi_pmicRtc_is_time_valid
@par Summary
Checks if the Julian time value is valid.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  RtcMode          Display mode; see #EFI_PM_RTC_DISPLAY_TYPE 
for details.
  @param[in]  Time             Time in Julian format; see 
#EFI_PM_RTC_JULIAN_TYPE for details.
  @param[out] Valid            Whether the time is valid.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
void (EFIAPI *EFI_PM_RTC_IS_TIME_VALID)(
	IN UINT32 PmicDeviceIndex, 
	IN EFI_PM_RTC_DISPLAY_TYPE RtcMode,
	IN CONST EFI_PM_RTC_JULIAN_TYPE *Time,
	OUT BOOLEAN *Valid
	);

/* EFI_PM_RTC_GET_TIME_VALUE_RAW */ 
/** @ingroup efi_pmicRtc_get_time_value_raw
@par Summary
Gets the raw RTC time value in seconds.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[out] TimeValueRaw     Time in seconds.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_GET_TIME_VALUE_RAW)(
	IN UINT32 PmicDeviceIndex, 
	OUT UINT32 *TimeValueRaw
	);

/* EFI_PM_RTC_SET_TIME_VALUE_RAW */ 
/** @ingroup efi_pmicRtc_set_time_value_raw
@par Summary
Sets the raw RTC time value in seconds.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  TimeValueRaw     Time in seconds.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_SET_TIME_VALUE_RAW)(
	IN UINT32 PmicDeviceIndex, 
	IN UINT32 TimeValueRaw
	);


/* EFI_PM_RTC_ALARM_ENABLE_STATUS */ 
/** @ingroup efi_pm_rtc_alarm_enable_status
@par Summary
Reads the alarm enabled status

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[out] Enabled     TRUE: Enabled, FALSE: Not Enabled

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_ALARM_ENABLE_STATUS)
(
 IN  UINT32  PmicDeviceIndex,
 OUT BOOLEAN *Enabled
 );


/* EFI_PM_RTC_ALARM_EXPIRE_STATUS */ 
/** @ingroup efi_pm_rtc_alarm_expire_status
@par Summary
Reads the alarm expired status

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ExpireStatus     TRUE: Expired, FALSE: Not expired

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_RTC_ALARM_EXPIRE_STATUS)
(
 IN  UINT32  PmicDeviceIndex,
 OUT BOOLEAN *ExpireStatus
 );

/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicRtc_protocol
@par Summary
Qualcomm PMIC Real-Time Clock (RTC) Protocol interface.

@par Parameters
@inputprotoparams{pmic_rtc_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_RTC_PROTOCOL {
  UINT64                              Revision;
  EFI_PM_RTC_GET_DISPLAY_MODE         GetDisplayMode;
  EFI_PM_RTC_SET_DISPLAY_MODE         SetDisplayMode;
  EFI_PM_RTC_GET_TIME_VALUE           GetTimeValue;
  EFI_PM_RTC_SET_TIME_VALUE           SetTimeValue;
  EFI_PM_RTC_IS_TIME_VALID            IsTimeValid;
  EFI_PM_RTC_GET_TIME_VALUE_RAW       GetTimeValueRaw;
  EFI_PM_RTC_SET_TIME_VALUE_RAW       SetTimeValueRaw;
  EFI_PM_RTC_ALARM_ENABLE_STATUS      AlarmEnableStatus;
  EFI_PM_RTC_ALARM_EXPIRE_STATUS      AlarmExpireStatus;

};

#endif	/* __EFIPMICRTC_H__ */
