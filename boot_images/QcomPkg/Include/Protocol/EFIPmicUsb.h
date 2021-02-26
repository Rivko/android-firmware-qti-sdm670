/**
@file  EFIPmicUsb.h
@brief PMIC usb for UEFI.
*/
/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Inc Confidential and Proprietary.

=============================================================================*/

/*===========================================================================

                        EDIT HISTORY

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
10/12/17   sv      Updated to support micro USB.
06/20/16   sm      Updated EFI_PM_USB_OTG_STATUS_TYPE enum
06/02/16   sm      Added ccout_out_hiz_sts in struct EFI_PM_USB_TYPEC_PORT_STATUS
05/04/16   sm      Added API to return charger presence
03/28/16   sm      Removed GetHVDCP API
03/07/16   sm      Added TypeC related APIs
01/30/15   al      New file.
===========================================================================*/

#ifndef __EFI_PMIC_USB_H__
#define __EFI_PMIC_USB_H__

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
/** @addtogroup efi_pmicUsb_constants
@{ */
/**
Protocol version.
*/

#define PMIC_USB_REVISION 0x0000000000010009
/** @} */ /* end_addtogroup efi_pmicUsb_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicUsb_protocol */
#define EFI_PMIC_USB_PROTOCOL_GUID \
    { 0x6adb9972, 0x83d, 0x4be4,  { 0xbb, 0x74, 0xc7, 0x25, 0xa7, 0x30, 0x61, 0x56 } }

/** @cond */


/*===========================================================================

                        EXTERNAL VARIABLES

===========================================================================*/
/**
External reference to the PMIC USB Protocol GUID.
*/
extern EFI_GUID gQcomPmicUsbProtocolGuid;

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_USB_PROTOCOL   EFI_QCOM_PMIC_USB_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicUsb_data_types
@{ */


/**  Charging port types. **/
typedef enum EFI_PM_USB_CHGR_PORT_TYPE
{
  EFI_PM_USB_CHG_PORT_SDP,                            /**< Standard Downstream Port         */
  EFI_PM_USB_CHG_PORT_OTHER,                          /**< Other Charging Port              */
  EFI_PM_USB_CHG_PORT_CDP,                            /**< Charging Downstream Port         */
  EFI_PM_USB_CHG_PORT_DCP,                            /**< Dedicated Charging Port          */
  EFI_PM_USB_CHG_PORT_FLOAT_CHARGE,                   /**< Floating Charging Port           */
  EFI_PM_USB_CHG_PORT_QC_2P0,                         /**< Quick Charger 2.0 Charging Port  */
  EFI_PM_USB_CHG_PORT_QC_3P0,                         /**< Quick Charger 3.0 Charging Port  */
  EFI_PM_USB_CHG_ICL_OVERRIDE_LATCH,  /**< ICL is in Override latch mode    */
  EFI_PM_USB_CHG_PORT_INVALID                         /**< INVALID PORT                     */
}EFI_PM_USB_CHGR_PORT_TYPE;

typedef enum EFI_PM_USB_OTG_CTRL_TYPE
{
  EFI_PM_USB_OTG_CTR__CMD_REG_RID_DISABLED,  /**< Command register with RID Disabled*/
  EFI_PM_USB_OTG_CTR__PIN_CTRL_RID_DISABLED, /**< Pin control with RID Disabled     */
  EFI_PM_USB_OTG_CTR__CMD_REG_RID_ENABLED,   /**< Command register with RID Enabled */
  EFI_PM_USB_OTG_CTR__AUTO_OTG_RID_ENABLED,  /**< Auto OTG (RID Enabled)            */
  EFI_PM_USB_OTG_CTR__INVALID
}EFI_PM_USB_OTG_CTRL_TYPE;

/**  RID status **/
typedef enum
{
   EFI_PM_USB_USB_CHGPTH_RID_GND,
   EFI_PM_USB_USB_CHGPTH_RID_FLOAT,
   EFI_PM_USB_USB_CHGPTH_RID_A,
   EFI_PM_USB_USB_CHGPTH_RID_B,
   EFI_PM_USB_USB_CHGPTH_RID_C,
   EFI_PM_USB_USB_CHGPTH_RID_INVALID
}EFI_PM_USB_USB_RID_STS_TYPE;

/**
 * NOTE: USB TypeC Enum fields are not based on IPCat register bit-wise definition.
 *       Any update to Enum values should reflect to low level API headers also.
 *       i.e. pm_typec.h
 */
/**  TYPEC CC OUT Status **/
typedef enum
{
   EFI_PM_USB_TYPEC_CC_OUT_RESERVED,
   EFI_PM_USB_TYPEC_CC_OUT_CC1,
   EFI_PM_USB_TYPEC_CC_OUT_CC2,
   EFI_PM_USB_TYPEC_CC_OUT_OPEN,
   EFI_PM_USB_TYPEC_CC_OUT_INVALID
} EFI_PM_USB_TYPEC_CC_OUT_STATUS;

/**  TYPEC DFP Current Advertisement - from UFP perspective **/
typedef enum
{
   EFI_PM_USB_TYPEC_DFP_CURR_ADV_RESERVED,
   EFI_PM_USB_TYPEC_DFP_CURR_ADV_3A,             // 3.0A @ 5V
   EFI_PM_USB_TYPEC_DFP_CURR_ADV_1P5A,           // 1.5A @ 5V
   EFI_PM_USB_TYPEC_DFP_CURR_ADV_STD,            // 900mA @ 5V - STD TypeC USB Power
   EFI_PM_USB_TYPEC_DFP_CURR_ADV_INVALID
} EFI_PM_USB_TYPEC_DFP_CURR_ADV;

/**  TYPEC UFP Connection Type (Cable Connector) - from DFP perspective  **/
typedef enum
{
   EFI_PM_USB_TYPEC_CABLE_CONNECTOR_RESERVED,
   EFI_PM_USB_TYPEC_STATE_UNATTACHED,            // DFP_OPEN_OPEN
   EFI_PM_USB_TYPEC_UFP_ATTACHED,                // DFP_RD_OPEN
   EFI_PM_USB_TYPEC_PWR_CABLE_NO_UFP_ATTCHED,    // DFP_RA_OPEN
   EFI_PM_USB_TYPEC_PWR_CABLE_UFP,               // DFP_RD_RA_VCONN
   EFI_PM_USB_TYPEC_AUDIO_ADAP_MODE,             // DFP_RA_RA
   EFI_PM_USB_TYPEC_DEBUG_ACCESSORY_MODE,        // DFP_RD_RD
   // DFP-to-DFP and UFP-to-UFP are undetectable states
   EFI_PM_USB_TYPEC_CABLE_CONNECTOR_INVALID
} EFI_PM_USB_TYPEC_UFP_CONNECTOR_TYPE;


/** For USB Operation/Mode types */
typedef enum
{
    EFI_PM_USB_CONNECTOR_TYPEC,
    EFI_PM_USB_CONNECTOR_MICRO_USB,
    EFI_PM_USB_CONNECTOR_INVALID
} EFI_PM_USB_CONNECTOR_TYPE;

/** For uUSB Connection types */
typedef enum
{
    EFI_PM_USB_MICROUSB_MODE_GROUND_VBUS,      /* Micro USB mode - Ground on CC1 when VBUS is present */
    EFI_PM_USB_MICROUSB_MODE_FLOAT_NOVBUS,     /* Micro USB mode - Float on CC1 when VBUS is not present */
    EFI_PM_USB_MICROUSB_MODE_GROUND_NOVBUS,    /* Micro USB mode - Ground on CC1 when VBUS is not present */
    EFI_PM_USB_MICROUSB_MODE_INVALID
} EFI_PM_USB_MICROUSB_MODE;

/**  TYPEC Port status on attach of USB cable **/
typedef struct
{
    EFI_PM_USB_TYPEC_CC_OUT_STATUS         cc_out_sts;
    EFI_PM_USB_TYPEC_DFP_CURR_ADV          dfp_curr_adv;
    EFI_PM_USB_TYPEC_UFP_CONNECTOR_TYPE    ufp_conn_type;
    BOOLEAN                                vbus_sts;
    BOOLEAN                                vbus_err_sts;
    BOOLEAN                                debounce_done_sts;
    BOOLEAN                                vconn_oc_sts;
    BOOLEAN                                ccout_out_hiz_sts;
    EFI_PM_USB_CONNECTOR_TYPE              usb_connector;
    EFI_PM_USB_MICROUSB_MODE               u_usb_mode;
} EFI_PM_USB_TYPEC_PORT_STATUS;

/**  TYPEC (local) Port Connection State.
 *
 * NOTE: USB TypeC RT_STS are valid irrespective of IRQ enabled for particular interrupt.
 *       UFP/DFP 'detect' status is HIGH as long as valid connection on TypeC Port, but 'detach'
 *       is PULSEd, so it's not reliable when Polling on these status.
 */
typedef enum
{
    EFI_PM_USB_TYPEC_CONNECT_MODE_NONE,
    EFI_PM_USB_TYPEC_CONNECT_MODE_DFP,
    EFI_PM_USB_TYPEC_CONNECT_MODE_UFP,
    EFI_PM_USB_TYPEC_CONNECT_MODE_INVALID
} EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS;

/**  TYPEC VConn Current Limit **/
typedef enum
{
   EFI_PM_USB_TYPEC_VCONN_ILIMIT_300MA,
   EFI_PM_USB_TYPEC_VCONN_ILIMIT_500MA,
   EFI_PM_USB_TYPEC_VCONN_ILIMIT_INVALID
} EFI_PM_USB_TYPEC_VCONN_ILIMIT;

/**  TYPEC VConn Soft Start Pull Down Current mA - SS_PD_IMA **/
typedef enum
{
   EFI_PM_USB_TYPEC_VCONN_SS_PD_35MA,
   EFI_PM_USB_TYPEC_VCONN_SS_PD_88MA,
   EFI_PM_USB_TYPEC_VCONN_SS_PD_160MA,
   EFI_PM_USB_TYPEC_VCONN_SS_PD_248MA,
   EFI_PM_USB_TYPEC_VCONN_SS_PD_INVALID
} EFI_PM_USB_TYPEC_VCONN_SS_PD_IMA;

/**  TYPEC VConn Enable Control **/
typedef enum
{
   EFI_PM_USB_TYPEC_VCONN_HW_CTRL,
   EFI_PM_USB_TYPEC_VCONN_SW_CTRL_DISABLE,
   EFI_PM_USB_TYPEC_VCONN_SW_CTRL_ENABLE,
   EFI_PM_USB_TYPEC_VCONN_EN_CTRL_INVALID
} EFI_PM_USB_TYPEC_VCONN_EN_CTRL;

/**  TYPEC VCONN Config **/
typedef struct
{
    EFI_PM_USB_TYPEC_VCONN_ILIMIT      vconn_curr_lim;
    EFI_PM_USB_TYPEC_VCONN_SS_PD_IMA   ss_pd_curr;
    EFI_PM_USB_TYPEC_VCONN_EN_CTRL     vconn_en_ctrl;
    BOOLEAN                            vconn_oc_en;
} EFI_PM_USB_TYPEC_VCONN_CONFIG;

/**  TYPEC Port Role **/
typedef enum
{
    EFI_PM_USB_TYPEC_ROLE_DRP,
    EFI_PM_USB_TYPEC_ROLE_DFP,
    EFI_PM_USB_TYPEC_ROLE_UFP,
    EFI_PM_USB_TYPEC_ROLE_INVALID
} EFI_PM_USB_TYPEC_PORT_ROLE;

/**  OTG status type **/
typedef enum
{
    EFI_PM_USB_OTG_ENABLED,
    EFI_PM_USB_OTG_DISABLED,
    EFI_PM_USB_OTG_ERROR,
    EFI_PM_USB_OTG_EXECUTING_ENABLE,
    EFI_PM_USB_OTG_EXECUTING_DISABLE,
    EFI_PM_USB_OTG_STATUS_INVALID
} EFI_PM_USB_OTG_STATUS_TYPE;

/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/* EFI_PMIC_USB_USBIN_VALID */
/** @ingroup efi_pmicUsb_usbin_valid
  @par Summary
  Indicates whether the VBUS on PMIC is high or low.

  @param[in]  PmicDeviceIndex    0:Primary, 1:Secondary
  @param[out] Valid              TRUE if VBUS high else FALSE.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_USBIN_VALID)(
  OUT BOOLEAN *Valid
);

/* EFI_PMIC_USB_CHARGER_PORT_TYPE */
/** @ingroup efi_pmic_usb_charger_port_type
  @par Summary
  reads charger port type.

  @param[in]  PmicDeviceIndex    Primary: 0, Secondary:1
  @param[out] PortType           Charging port type.
                                 For more info refer enum EFI_PM_USB_CHGR_PORT_TYPE

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_CHARGER_PORT_TYPE)(
  OUT EFI_PM_USB_CHGR_PORT_TYPE    *PortType
);

/**@ingroup efi_pmic_usb_config_otg
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
EFI_STATUS (EFIAPI *EFI_PMIC_USB_SET_OTG_I_LIMIT)(
  IN  UINT32                      ImAmp
);

/**@ingroup efi_pmic_usb_enable_otg
  @par Summary
  This API enables/disables OTG via command register

  @param[in]  PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]  Enable            TRUE: Enables; FALSE: Disables itv

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_ENABLE_OTG)(
  IN  BOOLEAN  Enable
);

/**@ingroup efi_pmic_usb_otg_status
  @par Summary
  This API returns the OTG status

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  OtgStatus         OTG Status, see EFI_PM_USB_OTG_STATUS_TYPE for more details

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_GET_OTG_STATUS)(
  OUT EFI_PM_USB_OTG_STATUS_TYPE  *OtgStatus
);

/**@ingroup efi_pmic_usb_get_rid_status
  @par Summary
  This API returns RID status

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  RidStsType        RID status as detected by PMIC. Refer #EFI_PM_USB_RID_STS_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_GET_RID_STATUS)(
  OUT  EFI_PM_USB_USB_RID_STS_TYPE   *RidStsType
);

/**@ingroup efi_pmic_usb_typec_get_port_state
  @par Summary
  This API returns Type-C port status.

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  TypeCStatus        Type-C port status

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_TYPEC_GET_PORT_STATE)(
  OUT  EFI_PM_USB_TYPEC_PORT_STATUS  *TypeCStatus
);

/**@ingroup efi_pmic_usb_typec_get_connect_state
  @par Summary
  This API returns Type-C port Connection status - in terms of Port Role.

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[out]  TypeCConnStatus   Type-C port connection status

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_TYPEC_GET_CONNECT_STATE)(
  OUT  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS  *TypeCConnStatus
);

/**@ingroup efi_pmic_usb_typec_set_vconn
  @par Summary
  This API sets VConn configs.

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]   VconnConfig       VConn Config set

  @return8
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_TYPEC_SET_VCONN)(
  IN  EFI_PM_USB_TYPEC_VCONN_CONFIG  VconnConfig
);

/**@ingroup efi_pmic_usb_typec_set_port_role
  @par Summary
  This API sets Type-C port role.

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]   PortRole          AUTO_DRP, DFP or UFP

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_TYPEC_SET_PORT_ROLE)(
  IN  EFI_PM_USB_TYPEC_PORT_ROLE  PortRole
);

/**@ingroup efi_pmic_usb_typec_enable
  @par Summary
  This API enables/disables Type-C periph.

  @param[in]   PmicDeviceIndex   0:Primary, 1:Secondary
  @param[in]   TypeCEnable       TRUE:Enable, FALSE:Disable

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_TYPEC_ENABLE)(
  IN  BOOLEAN  TypeCEnable
);

/**@ingroup efi_pmic_usb_get_charger_presence
  @par Summary
  This API returns charger presence status

  @param[out]  pChargerPresent   TRUE:Charger Present, FALSE:Charger not present

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_GET_CHARGER_PRESENCE)(
  OUT BOOLEAN  *pChargerPresent
);

/**@ingroup efi_pmic_usb_get_port_role_set_time
  @par Summary
  This API returns counter value when Port role was set using SetPortRole API

  @param[out]  pPortRoleSetTime   counter value

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_USB_GET_PORT_ROLE_SET_TIME)
(
  OUT UINT64  *pPortRoleSetTime
);


/*===========================================================================

                        PMIC USB PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup efi_pmicUsb_protocol
@par Summary
Qualcomm PMIC USB Protocol interface.

@par Parameters
@inputprotoparams{pmic_usb_proto_params.tex}
*/

struct _EFI_QCOM_PMIC_USB_PROTOCOL {
  UINT64                                        Revision;
  EFI_PMIC_USB_USBIN_VALID                      UsbinValid;
  EFI_PMIC_USB_CHARGER_PORT_TYPE                ChargerPort;
  EFI_PMIC_USB_SET_OTG_I_LIMIT                  SetOtgILimit;
  EFI_PMIC_USB_ENABLE_OTG                       EnableOtg;
  EFI_PMIC_USB_GET_OTG_STATUS                   GetOtgStatus;
  /* TypeC USB Protocol APIs */
  EFI_PMIC_USB_TYPEC_GET_PORT_STATE             UsbCGetPortState;       // Get Type-C Port state
  EFI_PMIC_USB_TYPEC_GET_CONNECT_STATE          UsbCGetConnectState;    // Get Type-C Connection state
  EFI_PMIC_USB_TYPEC_SET_VCONN                  UsbCSetVConn;           // Set Type-C VCONN config
  EFI_PMIC_USB_TYPEC_SET_PORT_ROLE              UsbCSetPortRole;        // Set Type-C Port Role
  EFI_PMIC_USB_TYPEC_ENABLE                     UsbCEnable;             // Enables/Disables TypeC periph
  EFI_PMIC_USB_GET_CHARGER_PRESENCE             GetChargerPresence;     // Returns if charger is present
  EFI_PMIC_USB_GET_PORT_ROLE_SET_TIME           GetPortRoleSetTime;
};

#endif  /* __EFI_PMIC_USB_H__ */

