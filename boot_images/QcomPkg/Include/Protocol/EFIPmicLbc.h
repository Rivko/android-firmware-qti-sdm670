/** 
@file  EFIPmicLbc.h
@brief PMIC LBC for UEFI.
*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Confidential and Proprietary.
    
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
04/04/14   al      Warning cleanup
12/05/13   ps      Created new protocol

=============================================================================*/

#ifndef __EFIPMICLBC_H__
#define __EFIPMICLBC_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmic_lbc_constants 
@{ */
/**
Protocol version.
*/

#define PMIC_LBC_REVISION 0x00010001
/** @} */ /* end_addtogroup efi_pmic_lbc_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmic_lbc_protocol */
#define EFI_PMIC_LBC_PROTOCOL_GUID \
    { 0x76b95dad, 0x1e66, 0x4d98, { 0x9c, 0xe7, 0xb2, 0x94, 0xd9, 0x59, 0x68, 0x95 } }

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC CLK Protocol GUID.
*/
extern EFI_GUID gQcomPmicLbcProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_LBC_PROTOCOL   EFI_QCOM_PMIC_LBC_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmic_lbc_data_types 
@{ */


/**  Attached charger type. **/
typedef enum 
{
   PM_UEFI_LBC_ATTACHED_CHGR__NONE,    /**< No charger. */
   PM_UEFI_LBC_ATTACHED_CHGR__BATT,    /**< Battery. */
   PM_UEFI_LBC_ATTACHED_CHGR__USB,     /**< USB path. */
   PM_UEFI_LBC_ATTACHED_CHGR__DCIN,    /**< DCIN path. */
} PM_UEFI_LBC_ATTACHED_CHGR_TYPE;

/**  Battery Temp HOT_THD type **/
typedef enum 
{
   PM_UEFI_LBC_BAT_TEMP_HOT_THD_LOW,    /**< 25% of VREF_BAT_THM */
   PM_UEFI_LBC_BAT_TEMP_HOT_THD_HIGH    /**< 35% of VREF_BAT_THM */
} PM_UEFI_LBC_BAT_TEMP_HOT_THD_TYPE;

/**  Battery Temp COLD_THD type **/
typedef enum 
{
   PM_UEFI_LBC_BAT_TEMP_COLD_THD_LOW,   /**< 70% of VREF_BAT_THM */
   PM_UEFI_LBC_BAT_TEMP_COLD_THD_HIGH    /**< 80% of VREF_BAT_THM */
} PM_UEFI_LBC_BAT_TEMP_COLD_THD_TYPE;
/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_LBC_INT */ 
/** @ingroup
  @par Summary
  Initializes LBC module

  @param[in]  PmicDeviceIndex    Primary: 0

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LBC_INT)(
    IN UINT32 PmicDeviceIndex
    );

/* EFI_PM_LBC_USB_OTG_ENABLE */ 
/** @ingroup
  @par Summary
  Enable/Disable Usb Otg charger path

  @param[in]  PmicDeviceIndex    Primary: 0
  @param[in]  Boolean         

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LBC_USB_OTG_ENABLE)(
    IN UINT32  PmicDeviceIndex, 
    IN BOOLEAN Enable
    );

/* EFI_PM_LBC_ENABLE_CHARGER */ 
/** @ingroup
  @par Summary
  Enables the charger module to charge the battery.
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[in]  ChargerEnable      If TRUE, enable the charger; if FALSE, disable 
                                 the charger.

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LBC_ENABLE_CHARGER)(
  IN UINT32  PmicDeviceIndex, 
  IN BOOLEAN ChargerEnable
);

/* EFI_PM_LBC_SET_USB_MAX_CURRENT */
/** @ingroup
  @par Summary
  Sets the maximum USB current.

  @param[in]  PmicDeviceIndex    Primary: 0
  @param[in]  MaxCurrent         Specifies the maximum current to charge the 
                                 battery in mA:
                                 - 0    -- Disable charging
                                 - 100  -- Typically used for a nonenumerated USB 
                                           PC charger and/or to trickle-charge a 
                                           battery
                                 - 500  -- Typically used for an enumerated USB 
                                           device that can source 500 mA
                                 - 500+ -- USB wall charger
                                 @tablebulletend

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LBC_SET_USB_MAX_CURRENT)(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 MaxCurrent
);

/* EFI_PM_LBC_GET_CHARGE_PATH */
/** @ingroup
  @par Summary
  Returns information about the charger attached to the device.
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[out] ChargerPath        Charger path; see #PM_UEFI_LBC_ATTACHED_CHGR_TYPE
                                 for details. 

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_GET_CHARGE_PATH)(
  IN  UINT32                          PmicDeviceIndex, 
  OUT PM_UEFI_LBC_ATTACHED_CHGR_TYPE *ChargerPath
);


/* EFI_PMIC_LBC_IS_BATTERY_PRESENT */
/** @ingroup efi_pmic_lbc_is_battery_present
  @par Summary
  Indicates whether the battery is present.
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[out] BatteryPresent     Whether or not the battery is present.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_IS_BATTERY_PRESENT)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *BatteryPresent
);

/* EFI_PMIC_LBC_USBIN_VALID */
/** @ingroup efi_pmic_lbc_usbin_valid
  @par Summary
  Indicates whether the USB is connected or not.
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[out] Valid              TRUE if VBUS high else FALSE.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_USBIN_VALID)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *Valid
);

/* EFI_PMIC_LBC_SET_VDD_MAX */
/** @ingroup efi_pmic_lbc_set_vdd_max
  @par Summary
  Sets value for vdd_max
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  ValueInmV        Specifies the maximum vdd for battery voltage
                               in mV.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_VDD_MAX)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmV
);

/* EFI_PMIC_LBC_SET_VDD_SAFE */
/** @ingroup efi_pmic_lbc_set_vdd_safe
  @par Summary
  Sets value for vdd_max
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  ValueInmV        Specifies the vdd_safe for battery voltage
                               in mV.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_VDD_SAFE)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmV
);

/* EFI_PMIC_LBC_SET_IBAT_MAX */
/** @ingroup efi_pmic_lbc_set_ibat_max
  @par Summary
  Sets value for ibat_max
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  ValueInmA        Specifies the limit for battery charging 
                               current in mA during fast charging.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_IBAT_MAX)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmA
);
 
/* EFI_PMIC_LBC_SET_IBAT_SAFE */
/** @ingroup efi_pmic_lbc_set_ibat_safe
  @par Summary
  Sets value for ibat_safe
 
  @param[in] PmicDeviceIndex   Primary: 0
  @param[in] ValueInmA         Specifies the limit for fast charge battery 
                               current maximum writeable value.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_IBAT_SAFE)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmA
);

/* EFI_PMIC_LBC_SET_ITRICKLE */
/** @ingroup efi_pmic_lbc_set_itrickle
  @par Summary
  Sets value for ibat_itrickle
 
  @param[in] PmicDeviceIndex  Primary: 0
  @param[in] ValueInmA        Specifies the Battery charging current during 
                              ATC(auto-trickle-charge).

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_ITRICKLE)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmA
);

/* EFI_PMIC_LBC_SET_VIN_MIN */
/** @ingroup efi_pmic_lbc_set_vin_min
  @par Summary
  Sets value for vin_min
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  ValueInmV        Specifies the value for vin_min.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_VIN_MIN)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmV
);

  /* EFI_PMIC_LBC_CONFIG_BTC */
/** @ingroup efi_pmic_lbc_config_btc
  @par Summary
  Configures battery temperature comparator
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  EnableBtc        TRUE to enable BTC else FALSE.
  @param[in]  ColdThd          Cold Threshold limit; see #PM_UEFI_LBC_BAT_TEMP_COLD_THD_TYPE
                               for details. 
  @param[in]  HotThd           Hot Threshold limit; see #PM_UEFI_LBC_BAT_TEMP_HOT_THD_TYPE
                               for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_CONFIG_BTC)(
  IN  UINT32                              PmicDeviceIndex,
  IN  BOOLEAN                             EnableBtc,
  IN  PM_UEFI_LBC_BAT_TEMP_COLD_THD_TYPE ColdThd,
  IN  PM_UEFI_LBC_BAT_TEMP_HOT_THD_TYPE  HotThd
);

/* EFI_PM_MISC_CONFIG_USB_ID_PULLUP */ 
/** @ingroup
  @par Summary
  Enables/disables the usb_id pullup
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[in]  Pullup             If TRUE, sets the usb_id pull
                                 up and lvdet; if FALSE,
                                 disables the usb_id pull up and
                                 lvdet.

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_MISC_CONFIG_USB_ID_PULLUP)(
  IN UINT32  PmicDeviceIndex, 
  IN BOOLEAN Pullup
);

/** @ingroup
  @par Summary
  Returns if end-of-charge is reached.
 
  @param[in]   PmicDeviceIndex    Primary: 0
  @param[out]  pReachedEoc        whether or not EOC is reached

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_LBC_IS_EOC_REACHED)(
  IN UINT32   PmicDeviceIndex, 
  OUT BOOLEAN *pReachedEoc
);

/* EFI_PMIC_LBC_EN_IDC_MAX */
/** @ingroup efi_pmic_lbc_en_idc_max
  @par Summary
  This enables or disables input current limiting
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  Enable           TRUE enables and FALSE disables input current limiting

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_EN_IDC_MAX)(
  IN  UINT32  PmicDeviceIndex,
  IN  BOOLEAN Enable
);

/* EFI_PMIC_LBC_SET_IDC_MAX */
/** @ingroup efi_pmic_lbc_set_idc_max
  @par Summary
  Sets the maximum value of current
 
  @param[in]  PmicDeviceIndex  Primary: 0
  @param[in]  ValueInmA        Max current to set from range of 100mA to 2500mA

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LBC_SET_IDC_MAX)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  ValueInmA
);
/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmic_lbc_protocol
@par Summary
Qualcomm PMIC LBC Protocol interface.

@par Parameters
@inputprotoparams{pmic_Lbc_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_LBC_PROTOCOL {
  UINT64                               Revision;
  EFI_PM_LBC_INT                       LbcInit;
  EFI_PM_LBC_ENABLE_CHARGER            EnableCharger;
  EFI_PM_LBC_SET_USB_MAX_CURRENT       SetUsbMaxCurrent;
  EFI_PMIC_LBC_GET_CHARGE_PATH         GetChargePath;
  EFI_PMIC_LBC_IS_BATTERY_PRESENT      IsBatteryPresent;
  EFI_PMIC_LBC_USBIN_VALID             UsbinValid;
  EFI_PMIC_LBC_SET_VDD_MAX             SetVddMax;
  EFI_PMIC_LBC_SET_VDD_SAFE            SetVddSafe;
  EFI_PMIC_LBC_SET_IBAT_MAX            SetIbatMax;
  EFI_PMIC_LBC_SET_IBAT_SAFE           SetIbatSafe;
  EFI_PMIC_LBC_SET_ITRICKLE            SetItrickle;
  EFI_PMIC_LBC_SET_VIN_MIN             SetVinMin;
  EFI_PMIC_LBC_CONFIG_BTC              ConfigBtc;
  EFI_PM_MISC_CONFIG_USB_ID_PULLUP     ConfigUsbPullup;
  EFI_PM_LBC_IS_EOC_REACHED            IsEocReached;
};

#endif  /* __EFIPMICLBC_H__ */
