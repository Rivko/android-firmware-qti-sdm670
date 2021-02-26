/** 
@file  EFIPmicSchg.h
@brief PMIC SCHG for UEFI.
*/
/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Inc Confidential and Proprietary.
    
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/24/17   pbitra  Added EFI_PMIC_SCHG_GET_CHARGER_CONNECTOR_TYPE API
07/21/17   va      Add Ship mode API
06/13/17   ll      Enable/disable HW AFP based on debug board detection
02/15/17   va      Add re run APSD 
02/06/17   cs      add config to enable/disable INOV
01/27/17   sm      Added support for pbs triggered AFP
01/23/17   sm      CFG parameter Structure changes
12/19/16   mr      Added API to get SCHG Charger PmicIndex
10/19/16   sm      removed EnableHvdcp cfg param from EFI_PM_SCHG_CFGDATA_TYPE struct
10/17/16   sm      Added SetChargeCmdBit() API 
                   Added elements in EFI_PM_SCHG_WDOG_STS_TYPE enum
10/13/16   cs      add protocol API for setting dcin current limit.
09/13/16   sm      removed unused config parameters from EFI_PM_SCHG_CFGDATA_TYPE
09/06/16   cs      wipower changes to remove 3.5 sec, not suspend DCIn
08/16/16   sm      Added API to enable/disable HW JEITA
07/18/16   sm      Added API to configure USB ICL options
07/15/16   cs      Adding Wipower support
06/24/16   sm      Added API to return if charging is enabled or not. 
06/23/16   sm      Added EnableHvdcp in EFI_PM_SCHG_CFGDATA_TYPE struct
06/23/16   va      Adding support for Charger Fg Peripheral dumps
06/09/16   sm      Added SupportHostMode in EFI_PM_SCHG_CFGDATA_TYPE
05/20/16   va      Update for struct mis match 
05/10/16   sm      Added back SchgExit for placeholder
04/21/16   sm      Added APIs to enable/disable and pet watchdog
03/28/16   sm      Removed GetHVDCP API and Enabled GetOtgSts and UsbinValid APIs
03/28/16   va      Adding Jeita Compensation params
03/21/16   va      Adding Exit API for clean up
02/19/16   sm      Initial draft

=============================================================================*/

#ifndef __EFIPMICSCHG_H__
#define __EFIPMICSCHG_H__

/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
/** @addtogroup efi_pmicSchg_constants 
@{ */
/**
Protocol version.
*/

#define PMIC_SCHG_REVISION 0x000000000001000B
/** @} */ /* end_addtogroup efi_pmicSchg_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicSchg_protocol */
#define EFI_PMIC_SCHG_PROTOCOL_GUID \
    { 0xae6ae96e, 0x483f, 0x42ae, { 0x9c, 0xc1, 0x9f, 0xac, 0x1b, 0x58, 0x47, 0x28 } } 


/** @cond */
/*===========================================================================

                        EXTERNAL VARIABLES

===========================================================================*/
/**
External reference to the PMIC SCHG Protocol GUID.
*/
extern EFI_GUID gQcomPmicSchgProtocolGuid;

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_SCHG_PROTOCOL   EFI_QCOM_PMIC_SCHG_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicSchg_data_types 
@{ */


/**  Attached charger type. **/
typedef enum 
{
   PM_SCHG_POWER_PATH__NONE,    /**< No charger. */
   PM_SCHG_POWER_PATH__BATTERY,    /**< Battery.    */
   PM_SCHG_POWER_PATH__USBIN,    /**< USB path.   */
   PM_SCHG_POWER_PATH__DCIN,    /**< DCIN path.  */
   PM_SCHG_POWER_PATH__INVALID,    /**< DCIN path.  */
} PM_SCHG_POWER_PATH_TYPE;

/**  Charging port types. **/
typedef enum EFI_PM_SCHG_CHGR_PORT_TYPE
{
  EFI_PM_SCHG_CHG_PORT_SDP_CHARGER,         /**< Standard Downstream Port         */
  EFI_PM_SCHG_CHG_PORT_OCP_CHARGER,         /**< Other Charging Port              */
  EFI_PM_SCHG_CHG_PORT_CDP_CHARGER,         /**< Charging Downstream Port         */
  EFI_PM_SCHG_CHG_PORT_DCP_CHARGER,         /**< Dedicated Charging Port          */
  EFI_PM_SCHG_CHG_PORT_FLOAT_CHARGER,       /**< Floating Charging Port           */
  EFI_PM_SCHG_CHG_PORT_QC_2P0,              /**< Quick Charger 2.0 Charging Port  */
  EFI_PM_SCHG_CHG_PORT_QC_3P0,              /**< Quick Charger 3.0 Charging Port  */
  EFI_PM_SCHG_CHG_PORT_ICL_OVERRIDE_LATCH,  /**< ICL is in Override latch mode    */
  EFI_PM_SCHG_CHG_PORT_INVALID = 0xFF,      /**< INVALID PORT                     */
}EFI_PM_SCHG_CHGR_PORT_TYPE;

typedef enum EFI_PM_SCHG_OTG_UVLO_SENSOR_SRC_TYPE
{
 EFI_PM_SCHG_OTG_UVLO_SENSOR__ANA,     /**<Analog Comparator       */
 EFI_PM_SCHG_OTG_UVLO_SENSOR__FG,      /**<Fuel Gauge ADC          */
 EFI_PM_SCHG_OTG_UVLO_SENSOR__BOTH,    /**<Or of both Analog and FG*/
 EFI_PM_SCHG_OTG_UVLO_SENSOR__INVALID  /**<Invalid type sensor     */
}EFI_PM_SCHG_OTG_UVLO_SENSOR_SRC_TYPE;

typedef enum EFI_PM_SCHG_OTG_CTRL_TYPE
{
  EFI_PM_SCHG_OTG_CTR__CMD_REG_RID_DISABLED,  /**< Command register with RID Disabled*/  
  EFI_PM_SCHG_OTG_CTR__PIN_CTRL_RID_DISABLED, /**< Pin control with RID Disabled     */
  EFI_PM_SCHG_OTG_CTR__CMD_REG_RID_ENABLED,   /**< Command register with RID Enabled */
  EFI_PM_SCHG_OTG_CTR__AUTO_OTG_RID_ENABLED,  /**< Auto OTG (RID Enabled)            */
  EFI_PM_SCHG_OTG_CTR__INVALID
}EFI_PM_SCHG_OTG_CTRL_TYPE;

typedef enum
{
  EFI_PM_SCHG_OTG_CFG_CONCURRENT_MODE_CFG,      /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
  EFI_PM_SCHG_OTG_CFG_OTG_EN_SRC_CFG,           /*0 = OTG Enable Source Command       1 = OTG Enable Source Type-C/RID*/
  EFI_PM_SCHG_OTG_CFG_ENABLE_OTG_IN_DEBUG_MODE, /*0 = Do not enable OTG in debug mode 1 = Enable OTG in debug mode*/
  EFI_PM_SCHG_OTG_CFG_INCREASE_DFP_TIME,        /*0 = DFP CC Cycle Time 2ms           1 = DFP CC Cycle Time 10ms*/
  EFI_PM_SCHG_OTG_CFG_INVALID,                  /*INVALID*/
}EFI_PM_SCHG_OTG_CFG_TYPE;

/**  Supported Input priorities. **/
typedef enum 
{
   EFI_PM_SCHG_INPUT_PRIORITY_DCIN,    /**< DCIN has priority.  */
   EFI_PM_SCHG_INPUT_PRIORITY_USBIN,   /**< USB has priority.   */
   EFI_PM_SCHG_INPUT_PRIORITY_INVALID, /**< INVALID input priority  */
} EFI_PM_SCHG_INPUT_PRIORITY_TYPE;

typedef enum
{
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_DEB_LV_ADAPTER,     /**< USBIN Input Collapse Option for LV adapter>*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_DEB_HV_ADAPTER,     /**< USBIN Input Collapse Option for HV adapter>*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_AICL_EN,            /**< TRUE:enabled, FALSE: disabled>*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_DEB_5V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 5V Select, Unregulated or 5V - 9V Adapter >*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_DEB_9V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 9V Adapter Select. >*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_DEB_9V_ADAPTER_EN,  /**< USBIN Input Collapse Glitch Filter for 9V Adapter Enable  >*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_OV_OPTION,          /**< FALSE = OV on USBIN will block DCIN current path; TRUE = OV on USBIN will not block DCIN current path>*/
   EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_INVALID             /**< INVALID >*/
}EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_TYPE;

typedef struct {

  BOOLEAN  PrintChargerAppDbgMsgToFile;
  /**< Flag to Enable/Disable Charger debug messages to File.  */

  BOOLEAN  PrintChargerAppDbgMsg;
  /**< Flag to Enable/Disable Charger debug messages.  */

  BOOLEAN  EnableChargerFgDump;
  /**< Flag to Enable/Disable Charger FG periperal debugs*/

  /* add other debug config here*/

}EFI_PM_SCHG_DEBUG_CFGDATA_TYPE;

typedef struct
{
  UINT32   ChargingTermCurrent;
  /**<  Charging termination current*/

  UINT32   ChgFccMax;
  /**< Value of FCC_MAX in mA */

  UINT32   JeitaCcCompCfg;
  /**< JEITA Charge Current Compensation when in battery temperature soft-limit in mA */

  UINT32   JeitaFvCompCfg;
  /**< JEITA Float Voltage Compensation when in battery temperature soft-limit in mA */

  UINT32  ChgFvMax;
  /**< Maximum Float Voltage */

  INT32   HwAfpHotThreshold;
  /**<Enable/Disable Charge Current compensation during soft cold*/

  INT32   HwAfpColdThreshold;
  /**<Enable/Disable Charge Current compensation during soft cold*/

  BOOLEAN EnableSkinTempINov;
  /**<Enable/Disable using skin temp for charging thermal imitiageion*/

  BOOLEAN EnableDieTempINov;
  /**<Enable/Disable using die temp for thermal mitigation*/

  BOOLEAN EnableDieTempCompINov;
  /**<Enable/Disable using die temp comparator for thermal mitigation*/

  EFI_PM_SCHG_DEBUG_CFGDATA_TYPE SchgDbgCfg;

}EFI_PM_SCHG_CFGDATA_TYPE;


/**< APSD cfg data type*/
typedef struct
{
  BOOLEAN bApsdRerun;
}EFI_SCHG_APSD_CFGDATA_TYPE;

/**< Hvdcp data type*/
typedef struct
{
  BOOLEAN bEnHvdcp;
}EFI_SCHG_HVDCP_CFGDATA_TYPE;

/**< JEITA types supported */
typedef enum
{
  EFI_PM_SCHG_JEITA_TYPE_HW,
  /**< Hardware JEITA */

  EFI_PM_SCHG_JEITA_TYPE_SW,
  /**< Software JEITA. */

  EFI_PM_SCHG_JEITA_TYPE_INVALID,
  /**< JEITA types invalid */
} EFI_PM_SCHG_JEITA_TYPE;

typedef enum {
  EFI_PM_SCHG_AICL_RERUN_TIME_0P2S,
  EFI_PM_SCHG_AICL_RERUN_TIME_0P4S,
  EFI_PM_SCHG_AICL_RERUN_TIME_0P8S,
  EFI_PM_SCHG_AICL_RERUN_TIME_1P6S,
  EFI_PM_SCHG_AICL_RERUN_TIME_CFG_INVALID, /*Invalid*/
}EFI_PM_SCHG_AICL_RERUN_TIME_CFG;

/**  OTG status type **/
typedef enum
{
    EFI_PM_SCHG_OTG_ENABLED,
    EFI_PM_SCHG_OTG_DISABLED,
    EFI_PM_SCHG_OTG_ERROR,
    EFI_PM_SCHG_OTG_EXECUTING_ENABLE,
    EFI_PM_SCHG_OTG_EXECUTING_DISABLE,
    EFI_PM_SCHG_OTG_STATUS_INVALID
} EFI_PM_SCHG_OTG_STATUS_TYPE;

typedef enum
{
  EFI_PM_SCHG_USB_ICL_MODE_SDP_2_0,        /**< Select USB 2.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
  EFI_PM_SCHG_USB_ICL_MODE_SDP_3_0,        /**< Select USB 3.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
  EFI_PM_SCHG_USB_ICL_MODE_WALL_CHARGERS, /**< Select USB 2.0 CFG(Default), Set USB51 Mode to 100 mA, set USBIN_MODE to HC Mode Current Level     >*/
  EFI_PM_SCHG_USB_ICL_MODE_INVALID
}EFI_PM_SCHG_USB_ICL_MODE_TYPE;

typedef enum
{
  EFI_PM_SCHG_WDOG_STS_BARK,                          /**< Indicates BARK WDOG Timer expired   >*/
  EFI_PM_SCHG_WDOG_STS_BITE_CHARGING_DISABLED,        /**< Indicates BITE WDOG Timer expired and Charging is Disabled  >*/
  EFI_PM_SCHG_WDOG_STS_BITE_CHARGING_NOT_DISABLED,    /**< Indicates BITE WDOG Timer expired but Charging is not Disabled  >*/
  EFI_PM_SCHG_WDOG_STS_INVALID,
}EFI_PM_SCHG_WDOG_STS_TYPE;


/** USB Connector type **/
typedef enum
{
    EFI_PM_SCHG_USB_CONNECTOR_TYPE_TYPEC,
    EFI_PM_SCHG_USB_CONNECTOR_TYPE_uUSB,
    EFI_PM_SCHG_USB_CONNECTOR_TYPE_INVALID
} EFI_PM_SCHG_USB_CONNECTOR_TYPE;

typedef struct
{
  BOOLEAN bChargingEnabled;
}EFI_PM_SCHG_CHARGING_STATUS;

typedef struct
{
  UINT32 PmicIndex;
  UINT32 SlaveIndex;
} EFI_PM_SCHG_PMIC_INFO;


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/* EFI_PM_SCHG_INT */ 
/** @ingroup
  @par Summary
  Initializes SCHG module

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_INIT)(
    IN UINT32 PmicDeviceIndex,
    IN EFI_PM_SCHG_CFGDATA_TYPE SchgCfgData,
    IN BOOLEAN bDbgBoard
    );

/* EFI_PM_SCHG_ENABLE_CHARGER */
/** @ingroup efi_pmSchg_enable_charger
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
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_CHARGER)(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN ChargerEnable
);

/* EFI_PMIC_SCHG_USBIN_VALID */
/** @ingroup efi_pmicSchg_usbin_valid
  @par Summary
  Indicates whether the VBUS on PMIC is high or low.

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] Valid              TRUE if VBUS high else FALSE.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_USBIN_VALID)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *Valid
);

/* EFI_PMIC_SCHG_GET_POWER_PATH */
/** @ingroup
  @par Summary
  Returns information about the charger attached to the device.

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] ChargerPath        Charger path; see #PM_SCHG_POWER_PATH_TYPE
                                 for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_GET_POWER_PATH)(
  IN  UINT32                   PmicDeviceIndex,
  OUT PM_SCHG_POWER_PATH_TYPE  *ChargerPath
);

/* EFI_PMIC_SCHG_IS_BATTERY_PRESENT */
/** @ingroup efi_pmicSchg_is_battery_present
  @par Summary
  Indicates whether the battery is present.

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] BatteryPresent     Whether or not the battery is present.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_IS_BATTERY_PRESENT)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *BatteryPresent
);


/* EFI_PMIC_SCHG_GET_CHARGER_PORT_TYPE */
/** @ingroup efi_pmic_schg_get_charger_port_type
  @par Summary
  reads charger port type.

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] PortType           Charging port type.
                                 For more info refer enum EFI_PM_SCHG_CHGR_PORT_TYPE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_GET_CHARGER_PORT_TYPE)(
  IN  UINT32                       PmicDeviceIndex,
  OUT EFI_PM_SCHG_CHGR_PORT_TYPE *PortType
);

/**@ingroup efi_pmic_schg_config_otg
  @par Summary
  This will configure OTG

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  OtgCfgType        OTG configuration parameters. Refer struct
                                #EFI_PM_SCHG_OTG_CFG_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_CONFIG_OTG)(
  IN  UINT32                    PmicDeviceIndex,
  IN  EFI_PM_SCHG_OTG_CFG_TYPE  OtgCfgType,
  IN  BOOLEAN                   SetValue
);

/**@ingroup efi_pmic_schg_config_otg
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
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_SET_OTG_I_LIMIT)(
  IN  UINT32                      PmicDeviceIndex,
  IN  UINT32                      ImAmp
);

/**@ingroup efi_pmic_schg_set_usb_max_current
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
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_SET_USB_MAX_CURRENT)(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxCurrent
);

/**@ingroup efi_pmic_schg_en_afp_mode
  @par Summary
  This will force device to go into AFP mode causing batfet to open and shutting down the device

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_AFP_MODE)(
  IN  UINT32                      PmicDeviceIndex
);

/**@ingroup efi_pmic_schg_set_charger_path_priority
  @par Summary
  This will set charger path priority

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  ChgrPath          Refer enum#EFI_PM_SCHG_CHGR_PATH_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_SET_INPUT_PRIORITY)(
  IN  UINT32                           PmicDeviceIndex,
  IN  EFI_PM_SCHG_INPUT_PRIORITY_TYPE  ChgrPath
);

/**@ingroup efi_pmic_schg_set_fcc_max_current
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
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_SET_FCC_MAX_CURRENT)(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxCurrentInMA
);

/**@ingroup efi_pmic_schg_set_fv_max_voltage
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
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_SET_FV_MAX_VOLTAGE)(
  IN UINT32          PmicDeviceIndex,
  IN UINT32          MaxVoltageInMv
);

/**@ingroup efi_pmic_schg_enable_otg
  @par Summary
  This API enables/disables OTG via command register

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  Enable            TRUE: Enables; FALSE: Disables itv

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_OTG)(
  IN  UINT32   PmicDeviceIndex,
  IN  BOOLEAN  Enable
);

/**@ingroup efi_pmic_schg_get_otg_ocp_sts
  @par Summary
  Returns OTG Over-current protection (OCP) status

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  Ok                OTG OCP is in good standing, FLASE: OTG OCP needs to be re-enabled

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_GET_OTG_STATUS)(
  IN  UINT32                       PmicDeviceIndex,
  OUT EFI_PM_SCHG_OTG_STATUS_TYPE  *pOtgStatus
);

/**@ingroup efi_pmic_schg_config_aicl
  @par Summary
  This API can be used to configure various AICL parameters

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]   AiclCfgType       AICL cfg type. Refer #EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_TYPE for more info
  @param[in]   Set               TRUE or FALSE

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_CONFIG_AICL)(
  IN UINT32                                  PmicDeviceIndex,
  IN EFI_PM_SCHG_USB_CHGPTH_AICL_CFG_TYPE  AiclCfgType,
  IN BOOLEAN                                 Set
);

/* EFI_PMIC_SCHG_USB_SUSPEND */
/** @ingroup efi_pmic_schg_usb_suspend
  @par Summary
  Enables/ Disables USB suspend mode

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[in]  Enable             TRUE to enable and FALSE to disable USB suspend mode

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_USB_SUSPEND)(
  IN  UINT32   PmicDeviceIndex,
  IN BOOLEAN   Enable
);

/* EFI_PMIC_SCHG_ENABLE_CHG_WDOG */
/** @ingroup EFI_PMIC_SCHG_ENABLE_CHG_WDOG
  @par Summary
  Enables/ Disables charger watchdog

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[in]  Enable             TRUE to enable and FALSE to disable Charger watchdog

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_CHG_WDOG)(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN Enable
);

/* EFI_PMIC_SCHG_PET_CHG_WDOG */
/** @ingroup efi_pmic_schg_pet_chg_wdog
  @par Summary
  Pets charger watchdog

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_PET_CHG_WDOG)(
  IN  UINT32   PmicDeviceIndex
);

/* EFI_PMIC_SCHG_EN_WDOG_IF_CHARGING */
/** @ingroup efi_pmic_schg_en_wdog_if_charging
  @par Summary
  Enable watchdog if device already charging

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_EN_WDOG_IF_CHARGING)(
  IN  UINT32   PmicDeviceIndex
);

/* EFI_PMIC_SCHG_PET_CHG_WDOG */
/** @ingroup efi_pmic_schg_chgr_source_reinserted
  @par Summary
  Return if USB port was changed or not

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] Reinserted    if port changed then TRUE else FALSE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_CHGR_SOURCE_REINSERTED)(
  IN UINT32   PmicDeviceIndex,
  OUT BOOLEAN *Reinserted
);


/* EFI_PMIC_SCHG_RERUN_AICL */
/** @ingroup efi_pmic_schg_rerun_aicl
  @par Summary
  Return AICL

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_RERUN_AICL)(
  IN  UINT32   PmicDeviceIndex,
  IN  EFI_PM_SCHG_AICL_RERUN_TIME_CFG RerunTime
);

/* EFI_PMIC_SCHG_PRINT_AICL_STS */
/** @ingroup EFI_PmicSchgPrintAiclSts
@par Summary
Get AICL status

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[out] isAICLComplete     FALSE: 0, TRUE:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_PRINT_AICL_STS)(
IN  UINT32   PmicDeviceIndex,
OUT BOOLEAN  *IsAICLComplete
);

/* EFI_PMIC_SCHG_DUMP_PERIPHERAL */
/** @ingroup efi_pmic_schg_dump_pheripheral
  @par Summary
  Dump Charger Pheripheral

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_DUMP_PERIPHERAL)(
  VOID
);

/* EFI_PMIC_SCHG_ENABLE_HW_JEITA */
/** @ingroup EFI_PmicSchgEnableHWJeita
@par Summary
Enables HW JEITA

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  Enable             Disable: 0, Enable:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_JEITA)(
  IN UINT32                   PmicDeviceIndex, 
  IN EFI_PM_SCHG_JEITA_TYPE JeitaType
);

/* EFI_PMIC_SCHG_GET_CHARGING_STATUS */
/** @ingroup EFI_PmicSchgGetChargingStatus
@par Summary
Returns if charging is enabled/disabled

@param[in]   PmicDeviceIndex    Primary: 0, Secondary:1
@param[out]  pChargingEnabled    TRUE: Charging Enabled
                                 FALSE: Charging Disabled

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PMIC_SCHG_GET_CHARGING_STATUS)(
  UINT32 PmicDeviceIndex,
  OUT EFI_PM_SCHG_CHARGING_STATUS *pChargingStatus
);


/* EFI_PMIC_SCHG_DCIN_VALID */
/** @ingroup efi_pmicSchg_dcin_valid
@par Summary
Indicates whether the DC IN is plugged in or not

@param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
@param[out] Valid              TRUE if VBUS high else FALSE.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_DCIN_VALID)(
  IN  UINT32  PmicDeviceIndex,
  OUT BOOLEAN *Valid
);


/* EFI_PMIC_SCHG_DCIN_SUSPEND */
/** @ingroup efi_pmicSchg_dcin_suspend
@par Summary
to suspend dcin or not

@param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
@param[in]  Suspend            TRUE:to suspend, FALSE:not to suspend.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_DCIN_SUSPEND)(
  IN  UINT32  PmicDeviceIndex,
  IN  BOOLEAN Suspend
);

/* EFI_PMIC_SCHG_SET_DCIN_POWER */
/** @ingroup EFI_PmicSchgSetDCInPower
@par Summary
set dcin power limit to certain value

@param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
@param[in]  UINT32             DCIN power in uV

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_SET_DCIN_POWER)(
  IN  UINT32  PmicDeviceIndex,
  IN  UINT32  PowerInMicroWalts
);

/* EFI_PMIC_SCHG_SET_USB_ICL_MODE */
/** @ingroup EFI_PmicSchgSetUsbIclMode
@par Summary
Sets USBIN ICL Mode

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  IclModeType        See EFI_PM_SCHG_USB_ICL_MODE_TYPE for details
@param[in]  SetValue           Value to be set

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PMIC_SCHG_SET_USB_ICL_MODE)(
  IN UINT32                         PmicDeviceIndex,
  IN EFI_PM_SCHG_USB_ICL_MODE_TYPE  IclModeType
);

/* EFI_PMIC_SCHG_GET_CHG_WDOG_STATUS */
/** @ingroup EFI_PmicSchgGetChgWdogStatus
@par Summary
Returns charger WDOG status

@param[in]   PmicDeviceIndex    Primary: 0, Secondary:1
@param[OUT]  pWdogSts           See EFI_PM_SCHG_WDOG_STS_TYPE for more info

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PMIC_SCHG_GET_CHG_WDOG_STATUS)(
  IN UINT32                    PmicDeviceIndex,
  OUT EFI_PM_SCHG_WDOG_STS_TYPE *pWdogSts
);

/* EFI_PMIC_SCHG_EXIT */
/** @ingroup EFI_PmicSchgExit
@par Summary
Clean up Schg charger related
@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PMIC_SCHG_EXIT)(
  UINT32 PmicDeviceIndex
);

/* EFI_PMIC_SCHG_ENABLE_HW_JEITA */
/** @ingroup EFI_PmicSchgEnableHWJeita
@par Summary
Enables HW JEITA

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  Enable             Disable: 0, Enable:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_HW_JEITA)(
  IN UINT32       PmicDeviceIndex,
  IN BOOLEAN      Enable
);

/* EFI_PMIC_SCHG_TOGGLE_WIPOWER_SDLATCH */
/** @ingroup EFI_PmicSchgResetWipowerRstN
@par Summary
toggles SHDN_N_CLEAR_CMD to disable getting power from dcin when chargeOK is Low

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_TOGGLE_WIPOWER_SDLATCH)(
  IN UINT32       PmicDeviceIndex
);

/* EFI_PMIC_SCHG_SET_DCIN_CURRENT */
/** @ingroup EFI_PmicSchgSetDcInCurrent
@par Summary
Set DCIn input current limit for non-wipower DCIn Charging

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  DcInCurrentLimit   0 to 3000 mA

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_SET_DCIN_CURRENT)(
  IN UINT32       PmicDeviceIndex,
  IN UINT32       DcInCurrentLimit
);

/* EFI_PMIC_SCHG_SET_CHARGER_CMD_BIT */
/** @ingroup EFI_PmicSchgSetChargeCmdBit
@par Summary
Sets value for charger command bit

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  Enable             FALSE: 0, TRUE:1

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_SET_CHARGER_CMD_BIT)(
  IN UINT32       PmicDeviceIndex,
  IN BOOLEAN      Enable
);


/* EFI_PMIC_SCHG_GET_PMIC_INFO */
/** @ingroup EFI_PmicSchgGetPmicInfo
  @par Summary
  Provides SCHG charger PMIC Index.

  @param[out]  SchgPmicInfo     PMIC Info, on what SCHG charger resides

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- If input param is invalid. \n
  EFI_DEVICE_ERROR      -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_GET_PMIC_INFO)(
    OUT EFI_PM_SCHG_PMIC_INFO *SchgPmicInfo
);

/* EFI_PMIC_SCHG_RERUN_APSD */
/** @ingroup EFI_PmicSchgReRunApsd
@par Summary
Sets value for charger Re run APSD bit

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  pSchgApsdDataType See struct def

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_CONFIG_APSD)
(
  IN UINT32                      PmicDeviceIndex,
  IN EFI_SCHG_APSD_CFGDATA_TYPE *pSchgApsdDataType
);

/* EFI_PMIC_SCHG_RERUN_APSD */
/** @ingroup EFI_PmicSchgReRunApsd
@par Summary
Sets value for charger Re run APSD bit

@param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
@param[in]  pSchgHvdcpDataType See struct def

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_SCHG_CONFIG_HVDCP)
(
  IN UINT32                      PmicDeviceIndex,
  IN EFI_SCHG_HVDCP_CFGDATA_TYPE *pSchgHvdcpDataType
);


/**@ingroup efi_pmic_schg_en_ship_mode
  @par Summary
  This will force device to go into Ship mode causing batfet to open and shutting down the device

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_ENABLE_SHIP_MODE)(
  IN  UINT32                      PmicDeviceIndex
);

/* EFI_PMIC_SCHG_GET_CHARGER_CONNECTOR_TYPE */
/** @ingroup efi_pmic_schg_get_charger_connector_type
  @par Summary
  reads charger connector type.

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] ConnectorType      Charger connector type.
                                 For more info refer enum EFI_PM_SCHG_USB_CONNECTOR_TYPE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_SCHG_GET_CHARGER_CONNECTOR_TYPE)(
  IN  UINT32                       PmicDeviceIndex,
  OUT EFI_PM_SCHG_USB_CONNECTOR_TYPE *ConnectorType
);

/*===========================================================================

                        SCHG PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup efi_pmicSchg_protocol
@par Summary
Qualcomm PMIC SCHG Protocol interface.

@par Parameters
@inputprotoparams{pmic_schg_proto_params.tex}
*/

struct _EFI_QCOM_PMIC_SCHG_PROTOCOL {
  UINT64                                         Revision;
  EFI_PMIC_SCHG_INIT                              SchgInit;
  EFI_PMIC_SCHG_ENABLE_CHARGER                   EnableCharger;
  EFI_PMIC_SCHG_GET_POWER_PATH                   GetPowerPath;
  EFI_PMIC_SCHG_IS_BATTERY_PRESENT               IsBatteryPresent;
  EFI_PMIC_SCHG_GET_CHARGER_PORT_TYPE            GetChargerPortType;
  EFI_PMIC_SCHG_CONFIG_OTG                       ConfigOtg;
  EFI_PMIC_SCHG_SET_OTG_I_LIMIT                  SetOtgILimit;
  EFI_PMIC_SCHG_ENABLE_AFP_MODE                  EnableAfpMode;
  EFI_PMIC_SCHG_SET_INPUT_PRIORITY               SetInputPriority;
  EFI_PMIC_SCHG_SET_FCC_MAX_CURRENT              SetFccMaxCurrent;
  EFI_PMIC_SCHG_SET_FV_MAX_VOLTAGE               SetFvMaxVoltage;
  EFI_PMIC_SCHG_ENABLE_OTG                       EnableOtg;
  EFI_PMIC_SCHG_USB_SUSPEND                      UsbSuspend;
  EFI_PMIC_SCHG_ENABLE_JEITA                     EnableJeita;
  EFI_PMIC_SCHG_GET_OTG_STATUS                   GetOtgStatus;
  EFI_PMIC_SCHG_USBIN_VALID                      UsbinValid;
  EFI_PMIC_SCHG_SET_USB_MAX_CURRENT              SetUsbMaxCurrent;
  EFI_PMIC_SCHG_CHGR_SOURCE_REINSERTED           ChgrSourceReinserted;
  EFI_PMIC_SCHG_RERUN_AICL                       RerunAicl;
  EFI_PMIC_SCHG_DUMP_PERIPHERAL                  DumpPeripheral;  
  EFI_PMIC_SCHG_ENABLE_CHG_WDOG                  EnableChgWdog;
  EFI_PMIC_SCHG_PET_CHG_WDOG                     PetChgWdog;
  EFI_PMIC_SCHG_GET_CHARGING_STATUS              GetChargingStatus;
  EFI_PMIC_SCHG_DCIN_VALID                       DcinValid;
  EFI_PMIC_SCHG_DCIN_SUSPEND                     DcinSuspend;
  EFI_PMIC_SCHG_SET_DCIN_POWER                   SetDcinPower;
  EFI_PMIC_SCHG_EXIT                             SchgExit;
  EFI_PMIC_SCHG_SET_USB_ICL_MODE                 SetUsbIclMode;
  EFI_PMIC_SCHG_GET_CHG_WDOG_STATUS              GetChgWdogStatus;
  EFI_PMIC_SCHG_ENABLE_HW_JEITA                  EnableHwJeita;
  EFI_PMIC_SCHG_TOGGLE_WIPOWER_SDLATCH           ToggleWipowerSDLatch;
  EFI_PMIC_SCHG_SET_DCIN_CURRENT                 SetDcinCurrent;
  EFI_PMIC_SCHG_SET_CHARGER_CMD_BIT              SetChargeCmdBit;
  EFI_PMIC_SCHG_GET_PMIC_INFO                    SchgGetPmicInfo;
  EFI_PMIC_SCHG_CONFIG_APSD                      ConfigApsd;
  EFI_PMIC_SCHG_CONFIG_HVDCP                     ConfigHvDcp;
  EFI_PMIC_SCHG_ENABLE_SHIP_MODE                 EnableShipMode;  
  EFI_PMIC_SCHG_GET_CHARGER_CONNECTOR_TYPE       GetChargerConnectorType;

  //Used by WP only
  //EFI_PMIC_SCHG_PRINT_AICL_STS                 PrintAiclSts;
  //EFI_PMIC_SCHG_ENABLE_SHIP_MODE               EnableShipMode;
  //EFI_PMIC_SCHG_EN_WDOG_IF_CHARGING            EnWdogIfCharging;

};

#endif  /* __EFIPMICSCHG_H__ */
