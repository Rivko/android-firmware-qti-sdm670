/** 
@file  EFIPmicSmbchg.h
@brief PMIC smbchg for UEFI.
*/
/*=============================================================================
Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Inc Confidential and Proprietary.
    
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/27/16   sm      Added JEITA related APIs
12/01/15   va      Added charger fg peripheral dump
11/30/15   va      UEFI Factory ship mode support
11/20/15   ak      Added changes necessary to print AICL status 
08/24/15   al      Adding API to check for charger re-insertion and re-run AICL 
09/08/15   al      API to enable WDOG if device already in charging
05/21/15   sm      Added API to pet charger watchdog
04/30/15   al      Adding USB suspend API
02/09/15   al      Add API to config AICL
11/17/14   al      Adding API to read RID status and enable/disable OTG via command register
10/30/14   va      Added API for setting FvMAx and FCC
09/23/14   al      Adding API for charger path priority  
08/13/14   sm      Added APIS to enable Charging and set IUSB_MAX
08/11/14   al      Adding API for AFP mode
05/19/14   al      Added OTG API 
04/18/14   al      Updated copyright 
04/06/14   al      Initial draft

=============================================================================*/

#ifndef __EFIPMICSMBCHG_H__
#define __EFIPMICSMBCHG_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicSmbchg_constants 
@{ */
/**
Protocol version.
*/

#define PMIC_SMBCHG_REVISION 0x0000000000010008
/** @} */ /* end_addtogroup efi_pmicSmbchg_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicSmbchg_protocol */
#define EFI_PMIC_SMBCHG_PROTOCOL_GUID \
    { 0x2a774acb, 0x7260, 0x4d9a, { 0x91, 0x44, 0x65, 0x4a, 0xda, 0xd7, 0x9c, 0x80 } }

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC SMBCHG Protocol GUID.
*/
extern EFI_GUID gQcomPmicSmbchgProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_SMBCHG_PROTOCOL   EFI_QCOM_PMIC_SMBCHG_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicSmbchg_data_types 
@{ */


/**  Attached charger type. **/
typedef enum 
{
   PM_SMBCHG_PWR_PTH_NONE,    /**< No charger. */
   PM_SMBCHG_PWR_PTH_BATT,    /**< Battery.    */
   PM_SMBCHG_PWR_PTH_USB,     /**< USB path.   */
   PM_SMBCHG_PWR_PTH_DCIN,    /**< DCIN path.  */
} PM_SMBCHG_PWR_PTH_TYPE;

/**  Charging port types. **/
typedef enum EFI_PM_SMBCHG_CHGR_PORT_TYPE
{
  EFI_PM_SMBCHG_CHG_PORT_CDP,           /**< Charging Downstream Port */
  EFI_PM_SMBCHG_CHG_PORT_DCP,           /**< Dedicated Charging Port  */
  EFI_PM_SMBCHG_CHG_PORT_OTHER,         /**< Other Charging Port      */
  EFI_PM_SMBCHG_CHG_PORT_SDP,           /**< Standard Downstream Port */
  EFI_PM_SMBCHG_CHG_PORT_FMB_UART_ON,   /**< FMB UART ON              */
  EFI_PM_SMBCHG_CHG_PORT_FMB_UART_OFF,  /**< FMB UART OFF             */
  EFI_PM_SMBCHG_CHG_PORT_FMB_USB_ON,    /**< FMB USB ON               */
  EFI_PM_SMBCHG_CHG_PORT_FMB_USB_OFF,   /**< FMB USB OFF              */
  EFI_PM_SMBCHG_CHG_PORT_INVALID        /**< INVALID PORT             */
}EFI_PM_SMBCHG_CHGR_PORT_TYPE;


/**  Enum for high voltage dcp status. **/
typedef enum
{
   EFI_PM_SMBCHG_HVDCP_SEL_5V,     /**< 5V High Voltage DCP Enabled >*/
   EFI_PM_SMBCHG_HVDCP_SEL_9V,     /**< 9V High Voltage DCP Enabled >*/
   EFI_PM_SMBCHG_HVDCP_SEL_12V,    /**< 12V High Voltage DCP Enabled>*/
   EFI_PM_SMBCHG_HVDCP_SEL_20V,    /**< 20V High Voltage DCP Enabled>*/
   EFI_PM_SMBCHG_IDEV_HVDCP_SEL_A, /**< High Voltage DCP detected   >*/
   EFI_PM_SMBCHG_HVDCP_STS_INVALID /**< INVALID >*/
}EFI_PM_HVDCP_STS_TYPE;


typedef enum EFI_PM_SMBCHG_OTG_UVLO_SENSOR_SRC_TYPE
{
 EFI_PM_SMBCHG_OTG_UVLO_SENSOR__ANA,     /**<Analog Comparator       */
 EFI_PM_SMBCHG_OTG_UVLO_SENSOR__FG,      /**<Fuel Gauge ADC          */
 EFI_PM_SMBCHG_OTG_UVLO_SENSOR__BOTH,    /**<Or of both Analog and FG*/
 EFI_PM_SMBCHG_OTG_UVLO_SENSOR__INVALID  /**<Invalid type sensor     */
}EFI_PM_SMBCHG_OTG_UVLO_SENSOR_SRC_TYPE;

typedef enum EFI_PM_SMBCHG_OTG_CTRL_TYPE
{
  EFI_PM_SMBCHG_OTG_CTR__CMD_REG_RID_DISABLED,  /**< Command register with RID Disabled*/  
  EFI_PM_SMBCHG_OTG_CTR__PIN_CTRL_RID_DISABLED, /**< Pin control with RID Disabled     */
  EFI_PM_SMBCHG_OTG_CTR__CMD_REG_RID_ENABLED,   /**< Command register with RID Enabled */
  EFI_PM_SMBCHG_OTG_CTR__AUTO_OTG_RID_ENABLED,  /**< Auto OTG (RID Enabled)            */
  EFI_PM_SMBCHG_OTG_CTR__INVALID
}EFI_PM_SMBCHG_OTG_CTRL_TYPE;

typedef struct EFI_PM_SMBCHG_OTG_CFG_TYPE
{
  BOOLEAN                                 HvOtgProtection;          /**< high voltage OTG enable/disable        */
  BOOLEAN                                 OtgPinPol;                /**< OTG pin polling on active high or low  */
  EFI_PM_SMBCHG_OTG_CTRL_TYPE             OtgCtrl;                  /**< OTG control type                       */
  EFI_PM_SMBCHG_OTG_UVLO_SENSOR_SRC_TYPE  OtgUvloSensorSrc;         /**< under voltage lockout sensor source    */
}EFI_PM_SMBCHG_OTG_CFG_TYPE;


/**  Supported ChargerPath. **/
typedef enum 
{
   EFI_PM_SMBCHG_CHGR_PATH_USB,     /**< USB path.   */
   EFI_PM_SMBCHG_CHGR_PATH_DCIN,    /**< DCIN path.  */
   EFI_PM_SMBCHG_CHGR_PATH_INVALID, /**< INVALID CHARGER PATH  */
} EFI_PM_SMBCHG_CHGR_PATH_TYPE;


/**  RID status **/
typedef enum 
{
   EFI_PM_SMBCHG_USB_CHGPTH_RID_GND,
   EFI_PM_SMBCHG_USB_CHGPTH_RID_FLOAT,
   EFI_PM_SMBCHG_USB_CHGPTH_RID_A,
   EFI_PM_SMBCHG_USB_CHGPTH_RID_B,
   EFI_PM_SMBCHG_USB_CHGPTH_RID_C,
   EFI_PM_SMBCHG_USB_CHGPTH_RID_INVALID
}EFI_PM_SMBCHG_USB_RID_STS_TYPE;


typedef enum
{
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_DEB_LV_ADAPTER,     /**< USBIN Input Collapse Option for LV adapter>*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_DEB_HV_ADAPTER,     /**< USBIN Input Collapse Option for HV adapter>*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_AICL_EN,            /**< TRUE:enabled, FALSE: disabled>*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_DEB_5V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 5V Select, Unregulated or 5V - 9V Adapter >*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_DEB_9V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 9V Adapter Select. >*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_DEB_9V_ADAPTER_EN,  /**< USBIN Input Collapse Glitch Filter for 9V Adapter Enable  >*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_OV_OPTION,          /**< FALSE = OV on USBIN will block DCIN current path; TRUE = OV on USBIN will not block DCIN current path>*/
   EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_INVALID             /**< INVALID >*/
}EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_TYPE;

typedef struct
{
  UINT32   ChgVddSafeInmV;
  /**< Value of VDD_SAFE in mV */

  UINT32   ChgIbatSafeInmA;
  /**< Value of IBAT_SAFE in mA */

  BOOLEAN  RunFccCalibration;
  /**< Shutdown the device if bad battery condition is TRUE */

  UINT32 SDPMaxChargeCurrent;
  /**< Maximum charge current allowed for SDP port*/

  UINT32 CDPMaxChargeCurrent; 
  /**< Maximum charge current allowed for CDP port*/

  UINT32 DCPMaxChargeCurrent;
  /**< Maximum charge current allowed for DCP port*/

  UINT32 OtherChargerChargeCurrent;
  /**< Maximum charge current allowed for other charger type*/

  UINT32   ChargingTermCurrent;
  /**<  Charging termination current*/

  UINT32   ChgFccMax; //_SMAROO move to charger specific
  /**< Value of FCC_MAX in mA */

}EFI_PM_SMBCHG_CFGDATA_TYPE;

typedef enum
{
  EFI_PM_SMBCHG_JEITA_INT_STATUS_NORMAL,
  EFI_PM_SMBCHG_JEITA_INT_HARD_COLD_ENABLED,
  EFI_PM_SMBCHG_JEITA_INT_HARD_HOT_ENABLED,
  EFI_PM_SMBCHG_JEITA_INT_SOFT_COLD_ENABLED,
  EFI_PM_SMBCHG_JEITA_INT_SOFT_HOT_ENABLED,
  EFI_PM_SMBCHG_JEITA_INT_STATUS_INVALID,
  
}EFI_PM_SMBCHG_JEITA_INT_STATUS;

/**< JEITA types supported */
typedef enum 
{ 
  EFI_PM_SMBCHG_JEITA_TYPE_HW,
  /**< Hardware JEITA */

  EFI_PM_SMBCHG_JEITA_TYPE_SW,
  /**< Software JEITA. */

  EFI_PM_SMBCHG_JEITA_TYPE_INVALID,
  /**< JEITA types invalid */  
} EFI_PM_SMBCHG_JEITA_TYPE;
/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_SMBCHG_INT */ 
/** @ingroup
  @par Summary
  Initializes SMBCHG module

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_SMBCHG_INT)(
    IN UINT32 PmicDeviceIndex,
    IN EFI_PM_SMBCHG_CFGDATA_TYPE SmbchgCfgData
    );

/* EFI_PM_SMBCHG_ENABLE_CHARGER */ 
/** @ingroup efi_pmSmbchg_enable_charger
  @par Summary
  Enables the charger module to charge the battery.
 
  @param[in]  PmicDeviceIndex    Primary: 0
  @param[in]  ChargerEnable      If TRUE, enable the charger; if FALSE, disable 
                                 the charger.

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_SMBCHG_ENABLE_CHARGER)(
  IN UINT32  PmicDeviceIndex, 
  IN BOOLEAN ChargerEnable
);

/* EFI_PMIC_SMBCHG_USBIN_VALID */
/** @ingroup efi_pmicSmbchg_usbin_valid
  @par Summary
  Indicates whether the VBUS on PMIC is high or low.
 
  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] Valid              TRUE if VBUS high else FALSE.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_USBIN_VALID)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *Valid
);



/* EFI_PM_SMBCHG_GET_CHARGE_PATH */
/** @ingroup
  @par Summary
  Returns information about the charger attached to the device.
 
  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] ChargerPath        Charger path; see #PM_UEFI_SMBCHG_PWR_PTH_TYPE
                                 for details. 

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_GET_CHARGE_PATH)(
  IN  UINT32                       PmicDeviceIndex, 
  OUT PM_SMBCHG_PWR_PTH_TYPE       *ChargerPath
);


/* EFI_PMIC_SMBCHG_IS_BATTERY_PRESENT */
/** @ingroup efi_pmicSmbchg_is_battery_present
  @par Summary
  Indicates whether the battery is present.
 
  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] BatteryPresent     Whether or not the battery is present.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_IS_BATTERY_PRESENT)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *BatteryPresent
);


/* EFI_PMIC_SMBCHG_CHARGER_PORT_TYPE */
/** @ingroup efi_pmic_smbchg_charger_port_type
  @par Summary
  reads charger port type.
 
  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] PortType           Charging port type.
                                 For more info refer enum EFI_PM_SMBCHG_CHGR_PORT_TYPE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_CHARGER_PORT_TYPE)
(
  IN  UINT32                       PmicDeviceIndex,
  OUT EFI_PM_SMBCHG_CHGR_PORT_TYPE *PortType
);


/** @ingroup efi_pmic_smbchg_get_hvdcp
  @par Summary
  This will get hvdcp related status
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out] HvdcpSts          Reads HVDP status. Refer enum 
                                EFI_PM_HVDCP_STS for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_GET_HVDCP)
(
  IN  UINT32                 PmicDeviceIndex,
  IN  EFI_PM_HVDCP_STS_TYPE  HvdcpSts,
  OUT BOOLEAN                *Enable
);


/**@ingroup efi_pmic_smbchg_config_otg
  @par Summary
  This will configure OTG 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  OtgCfgType        OTG configuration parameters. Refer struct
                                #EFI_PM_SMBCHG_OTG_CFG_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_CONFIG_OTG)
(
  IN  UINT32                      PmicDeviceIndex,
  IN  EFI_PM_SMBCHG_OTG_CFG_TYPE  *OtgCfgType
);


/**@ingroup efi_pmic_smbchg_config_otg
  @par Summary
  This will set the OTG current limit. Valid values are 250 to 1000mAmp 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  ImAmp             Current limit in milliAmp. 
                                Valid values are 250 to 1000mAm

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_SET_OTG_I_LIMIT)
(
  IN  UINT32                      PmicDeviceIndex,
  IN  UINT32                      ImAmp
);


/**@ingroup efi_pmic_smbchg_set_usb_max_current
  @par Summary
  This will set the USB max current limit. Valid values are 100 to 2500mAmp 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  MaxCurrent        Current limit in milliAmp. 
                                Valid values are 250 to 2500mA

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_SET_USB_MAX_CURRENT)
(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxCurrent
);

/**@ingroup efi_pmic_smbchg_en_afp_mode
  @par Summary
  This will force device to go into AFP mode causing batfet to open and shutting down the device 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_ENABLE_AFP_MODE)
(
  IN  UINT32                      PmicDeviceIndex
);

/**@ingroup efi_pmic_smbchg_en_ship_mode
  @par Summary
  This will force device to go into Ship mode causing batfet to open and shutting down the device 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_ENABLE_SHIP_MODE)
(
  IN  UINT32                      PmicDeviceIndex
);


/**@ingroup efi_pmic_smbchg_set_charger_path_priority
  @par Summary
  This will set charger path priority 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  ChgrPath          Refer enum#EFI_PM_SMBCHG_CHGR_PATH_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_SET_CHARGER_PATH_PRIORITY)
(
  IN  UINT32                          PmicDeviceIndex,
  IN  EFI_PM_SMBCHG_CHGR_PATH_TYPE    ChgrPath 
);

/**@ingroup efi_pmic_smbchg_set_fcc_max_current
  @par Summary
  This will set the Battery Fast Charging max current limit. Valid values are 300 to 3000mAmp 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  MaxCurrentInMA    Current limit in milliAmp. 
                                Valid values are 300 to 3000mA

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_SET_FCC_MAX_CURRENT)
(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxCurrentInMA
);
/**@ingroup efi_pmic_smbchg_set_fv_max_voltage
  @par Summary
  This will set the Battery Floating Voltage . Valid values are 100 to 2500mAmp 
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  MaxVoltageInMv   MaxVoltage limit in mV. 
                                Valid values are 3600mV to 4500 mv

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_SET_FV_MAX_VOLTAGE)
(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxVoltageInMv
);


/**@ingroup efi_pmic_smbchg_enable_otg
  @par Summary
  This API enables/disables OTG via command register  
 
  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  Enable            TRUE: Enables; FALSE: Disables itv

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_ENABLE_OTG)
(
  IN  UINT32   PmicDeviceIndex,
  IN  BOOLEAN  Enable
);


/**@ingroup efi_pmic_smbchg_vbus_work_arnd
  @par Summary
  This API implements the software workaround required when high capacitance load is insterted
 
  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  Ok     OTG is in good standing, FLASE: OTG needs to be re-enabled

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_OTG_STS)
(
  IN  UINT32   PmicDeviceIndex,
  OUT BOOLEAN  *Ok
);


/**@ingroup efi_pmic_smbchg_get_rid_status
  @par Summary
  This API returns RID status   
 
  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  RidStsType        RID status as detected by PMIC. Refer #EFI_PM_SMBCHG_RID_STS_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef 
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_GET_RID_STATUS)
(
  IN   UINT32                           PmicDeviceIndex,
  OUT  EFI_PM_SMBCHG_USB_RID_STS_TYPE   *RidStsType
);


/**@ingroup efi_pmic_smbchg_config_aicl
  @par Summary
  This API can be used to configure various AICL parameters
 
  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]   AiclCfgType       AICL cfg type. Refer #EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_TYPE for more info
  @param[in]   Set               TRUE or FALSE

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef 
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_CONFIG_AICL)
(
  IN UINT32                                  PmicDeviceIndex,
  IN EFI_PM_SMBCHG_USB_CHGPTH_AICL_CFG_TYPE  AiclCfgType,
  IN BOOLEAN                                 Set
);

/* EFI_PMIC_SMBCHG_USB_SUSPEND */
/** @ingroup efi_pmic_smbchg_usb_suspend
  @par Summary
  Enables/ Disables USB suspend mode

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[in]  Enable             TRUE to enable and FALSE to disable USB suspend mode

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_USB_SUSPEND)
(
  IN  UINT32   PmicDeviceIndex,
  IN BOOLEAN   Enable
);

/* EFI_PMIC_SMBCHG_PET_CHG_WDOG */
/** @ingroup efi_pmic_smbchg_pet_chg_wdog
  @par Summary
  Pets charger watchdog

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_PET_CHG_WDOG)
(
  IN  UINT32   PmicDeviceIndex
);

/* EFI_PMIC_SMBCHG_EN_WDOG_IF_CHARGING */
/** @ingroup efi_pmic_smbchg_en_wdog_if_charging
  @par Summary
  Enable watchdog if device already charging

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_EN_WDOG_IF_CHARGING)
(
  IN  UINT32   PmicDeviceIndex
);

/* EFI_PMIC_SMBCHG_PET_CHG_WDOG */
/** @ingroup efi_pmic_smbchg_chgr_source_reinserted
  @par Summary
  Return if USB port was changed or not

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] Reinserted    if port changed then TRUE else FALSE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_CHGR_SOURCE_REINSERTED)
(
  IN UINT32   PmicDeviceIndex,
  OUT BOOLEAN *Reinserted
);


/* EFI_PMIC_SMBCHG_RERUN_AICL */
/** @ingroup efi_pmic_smbchg_rerun_aicl
  @par Summary
  Return AICL

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_RERUN_AICL)
(
  IN  UINT32   PmicDeviceIndex
);


/* EFI_PMIC_SMBCHG_PRINT_AICL_STS */
/** @ingroup EFI_PmicSmbchgPrintAiclSts
@par Summary
Get AICL status

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[out] isAICLComplete     FALSE: 0, TRUE:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SMBCHG_PRINT_AICL_STS)
(
IN  UINT32   PmicDeviceIndex,
OUT BOOLEAN  *IsAICLComplete
);

/* EFI_PMIC_SMBCHG_DUMP_PERIPHERAL */
/** @ingroup efi_pmic_smbchg_dump_pheripheral
  @par Summary
  Dump Charger Pheripheral 

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_DUMP_PERIPHERAL)
(
  IN  UINT32   PmicDeviceIndex
);

/* EFI_PMIC_SMBCHG_ENABLE_HW_JEITA */
/** @ingroup EFI_PmicSmbchgEnableHWJeita
@par Summary
Enables HW JEITA

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  Enable             Disable: 0, Enable:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_ENABLE_JEITA)
(
  IN UINT32                   PmicDeviceIndex, 
  IN EFI_PM_SMBCHG_JEITA_TYPE JeitaType
);

/* EFI_PMIC_SMBCHG_ENABLE_SW_JEITA */
/** @ingroup EFI_PmicSmbchgEnableSWJeita
@par Summary
Enables SW JEITA

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  Enable             Disable: 0, Enable:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_ENABLE_SW_JEITA)
(
  IN UINT32  PmicDeviceIndex, 
  IN BOOLEAN Enable
);

/* EFI_PMIC_SMBCHG_GET_HW_JEITA_INT_STATUS */
/** @ingroup EFI_PmicSmbchgGetHwJeitaIntStatus
@par Summary
Returns JEITA interrupt status 

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  pJeitaIntStatus    Jeita interrupt status 
                               see PM_SMBCHG_JEITA_INT_STATUS for details 

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SMBCHG_GET_HW_JEITA_INT_STATUS)
(
  UINT32                         PmicDeviceIndex, 
  EFI_PM_SMBCHG_JEITA_INT_STATUS *pJeitaIntStatus
);

/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicSmbchg_protocol
@par Summary
Qualcomm PMIC SMBCHG Protocol interface.

@par Parameters
@inputprotoparams{pmic_smbchg_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_SMBCHG_PROTOCOL {
  UINT64                                           Revision;
  EFI_PM_SMBCHG_INT                                SmbchgInit;
  EFI_PM_SMBCHG_ENABLE_CHARGER                     EnableCharger;
  EFI_PMIC_SMBCHG_USBIN_VALID                      UsbinValid;
  EFI_PMIC_SMBCHG_GET_CHARGE_PATH                  GetChargePath;
  EFI_PMIC_SMBCHG_IS_BATTERY_PRESENT               IsBatteryPresent;
  EFI_PMIC_SMBCHG_CHARGER_PORT_TYPE                ChargerPort;
  EFI_PMIC_SMBCHG_GET_HVDCP                        GetHvdcp;
  EFI_PMIC_SMBCHG_CONFIG_OTG                       ConfigOtg;
  EFI_PMIC_SMBCHG_SET_OTG_I_LIMIT                  SetOtgILimit;
  EFI_PMIC_SMBCHG_SET_USB_MAX_CURRENT              SetUsbMaxCurrent;
  EFI_PMIC_SMBCHG_ENABLE_AFP_MODE                  EnableAfpMode;
  EFI_PMIC_SMBCHG_SET_CHARGER_PATH_PRIORITY        SetChargerPathPriority;
  EFI_PMIC_SMBCHG_SET_FCC_MAX_CURRENT              SetFccMaxCurrent;
  EFI_PMIC_SMBCHG_SET_FV_MAX_VOLTAGE               SetFvMaxVoltage;
  EFI_PMIC_SMBCHG_ENABLE_OTG                       EnableOtg;
  EFI_PMIC_SMBCHG_OTG_STS                          OtgSts;
  EFI_PMIC_SMBCHG_GET_RID_STATUS                   GetRidStatus;
  EFI_PMIC_SMBCHG_CONFIG_AICL                      ConfigAicl;
  EFI_PMIC_SMBCHG_USB_SUSPEND                      UsbSuspend;
  EFI_PMIC_SMBCHG_PET_CHG_WDOG                     PetChgWdog;
  EFI_PMIC_SMBCHG_EN_WDOG_IF_CHARGING              EnWdogIfCharging;
  EFI_PMIC_SMBCHG_CHGR_SOURCE_REINSERTED           ChgrSourceReinserted;
  EFI_PMIC_SMBCHG_RERUN_AICL                       RerunAicl;
  EFI_PMIC_SMBCHG_PRINT_AICL_STS				   PrintAiclSts;
  EFI_PMIC_SMBCHG_ENABLE_SHIP_MODE                 EnableShipMode;
  EFI_PMIC_SMBCHG_DUMP_PERIPHERAL                  DumpPeripheral;
  EFI_PMIC_SMBCHG_ENABLE_JEITA                     EnableJeita;
  EFI_PMIC_SMBCHG_GET_HW_JEITA_INT_STATUS          GetHwJeitaIntStatus;

};

#endif  /* __EFIPMICSMBCHG_H__ */
