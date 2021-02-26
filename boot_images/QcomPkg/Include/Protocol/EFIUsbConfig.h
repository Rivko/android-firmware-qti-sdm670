/** 
  @file  EFIUsbConfig.h
  @brief QCOM_USB_CONFIG Protocol Interface.
*/
/*=============================================================================
  Copyright (c) 2011-2016, 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/06/17   pm       Vbus Enablement in USB Config Protocol
 05/23/16   ck       Update enum of QCOM_USB_MODE_TYPE to remove entries with the same numerical value.
 04/12/16   ck       Add the support to the USB init protocol
 02/23/16   ck       Add the support to query cable connection
 04/27/15   ck       Add init Hawker support
 02/27/15   amitg    UEFI Low Power Mode Updates
 11/19/14   ck       Add toggle USB mode support
 10/22/14   amitg    UEFI LPM support
 08/23/13   mliss    Added GetSSUsbfnConfig
 04/10/13   amitg    Limited the suport for number of cores since there is no FSUSB core for B-faily targets
 06/01/12   kameya   Support for multiple USB cores in host mode in UEFI.
 05/03/12   vishalo  Merge in Techpubs Doxygen change
 03/24/12   llg      (Tech Pubs) Edited/added Doxygen comments and markup.
 01/26/12   vishalo  Merge in Techpubs Doxygen change
 11/02/11   sbryksin Added initial APQ8064 revision
 10/25/11   llg      (Tech Pubs) Edited/added Doxygen comments and markup.
 08/11/11   sbryksin Changes for 8960v2 w/ HSIC
 05/11/11   sbryksin Added platform/core type selection for UsbFn
 03/30/11   sniedzie Created from PlatConfig.
=============================================================================*/
#ifndef _USB_CONFIG_PRO_H_
#define _USB_CONFIG_PRO_H_

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @cond */
/*  this is redefined below just before the protocol interface. 
  Protocol version.
*/
#define EFI_QCOM_USB_CONFIG_PROTOCOL_REVISION 0x00010000
/** @endcond */

/*  Protocol GUID definition */
/** @ingroup efi_usbConfig_protocol */
#define QCOM_USB_CONFIG_PROTOCOL_GUID \
  { \
    0xe722b03f, 0xb250, 0x42ce, {0x8e, 0xbd, 0x5b, 0xd5, 0x18, 0x12, 0xd0, 0x37 }}

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the QCOM USB Protocol GUID.
*/
extern EFI_GUID gQcomUsbConfigProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _QCOM_USB_CONFIG_PROTOCOL QCOM_USB_CONFIG_PROTOCOL;
/** @endcond */

/** @addtogroup efi_usbConfig_data_types 
@{ */
/**
  USB core id. 
*/
typedef enum {
  USB_CORE_0 = 0,      /**< Core0. */
  USB_CORE_1,          /**< Core1. */
  USB_CORE_2,          /**< Core2. */
  USB_CORE_3,          /**< Core3. */  
  USB_CORE_MAX_NUM     /**< Invalid Core. */
} QCOM_USB_CORE_NUM;

/**
  USB mode
*/
typedef enum {  
  /** USB mode type for existing USB Config APIs */  
  USB_HOST_MODE_EHCI = 0x00000000,     /**< host controller supporting EHCI interface */
  USB_HOST_MODE_XHCI = 0x00000001,     /**< host controller supporting XHCI interface */
  USB_DEVICE_MODE_HS = 0x00000002,     /**< device controller supporting ChipIdea device mode */
  USB_DEVICE_MODE_SS = 0x00000004,     /**< device controller supporting SNPS device mode */
  /** USB mode type for client to select configuration */
  USB_HOST_MODE      = 0x00000008,     /**< host mode */
  USB_DEVICE_MODE    = 0x00000010,     /**< Device mode */
  USB_DUAL_ROLE_MODE = 0x00000020,     /**< Dual Role */  
  /** invalid mode, used for error condition */
  USB_INVALID_MODE   = 0x00010000,     /**< invalid USB mode */
} QCOM_USB_MODE_TYPE;


/**
  USB core and mode.
*/
typedef enum {
  USB_CONFIG_HSUSB1, /**< High-speed USB 1. */
  USB_CONFIG_HSUSB2, /**< High-speed USB 2. */
  USB_CONFIG_HSUSB3, /**< High-speed USB 3. */
  USB_CONFIG_HSUSB4, /**< High-speed USB 4. */
  USB_CONFIG_HSIC,   /**< High-speed inter-chip. */
  USB_CONFIG_SSUSB1, /**< Super-speed USB 1. */
  USB_CONFIG_SSUSB2, /**< Super-speed USB 2. */
/** @cond */
  USB_CONFIG_CORE_MAX
/** @endcond */
} QCOM_USB_CONFIG_CORE_TYPE;
/** @} */ /* end_addtogroup efi_usbConfig_data_types */

/*
Vbus Status Type
*/

typedef enum {
  VBUS_STATUS_ENABLED = 0,
  VBUS_STATUS_DISABLED,
  VBUS_STATUS_UNSUPPORTED
} VBUS_STATUS_TYPE;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_START_CONTROLLER)(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  );
  
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_STOP_CONTROLLER)(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  );


  
/* QCOM_USB_CONFIG_PROTOCOL_GET_CORE_BASE_ADDR */ 
/** @ingroup efi_usbConfig_get_core_base_addr
  @par Summary
  Gets the USB core base address. When the USB is initialized successfully, 
  EFI_SUCCESS is returned.

  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]  CoreType       USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                             for details.
  @param[out] BaseAddr       USB core base address.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_CORE_BASE_ADDR)(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  OUT  UINTN                      *BaseAddr
  );


/* QCOM_USB_CONFIG_PROTOCOL_CONFIG_USB */ 
/** @ingroup efi_usbConfig_config_usb
  @par Summary
  Initializes the target for USB control.

  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]  Mode           USB mode; see #QCOM_USB_MODE_TYPE for details.
  @param[in]  CoreNum        USB corenum; see#QCOM_USB_NUM for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_CONFIG_USB)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  );

/* QCOM_USB_CONFIG_PROTOCOL_RESET_USB */ 
/** @ingroup efi_usbConfig_reset_usb
  @par Summary
  Resets the USB to the appropriate mode.

  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]  CoreType       USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                             for details.
  @param[in]  DeviceMode     Whether to reset the device.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_RESET_USB)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  IN QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  IN BOOLEAN                    DeviceMode
  );


/* QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONFIG */ 
/** @ingroup efi_usbConfig_get_usbfn_config
  @par Summary
  Gets the appropriate USBFn core type for each platform.

  @param[in]   This          Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[out]  CoreType      USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                             for details.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONFIG)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  );

/* QCOM_USB_CONFIG_PROTOCOL_GET_SS_USBFN_CONFIG */ 
/** @ingroup efi_usbConfig_get_usbfn_config
  @par Summary
  Gets the appropriate SS USBFn core type for each platform.

  @param[in]   This          Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[out]  CoreType      USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                             for details.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_SS_USBFN_CONFIG)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  );

/* QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONN_STATUS */ 
/** @ingroup efi_usbConfig_get_usbfn_config
  @par Summary
  Gets the port partner connection state for the appropriate Type-C Port

  @param[in]   This         Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]   CoreType		  USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                            for details.
  @param[out]  IsConnected	Is the port partner connected

  @return
  EFI_SUCCESS     --     Function completed successfully.
  Other           --     Function failed.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONN_STATUS)(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CONFIG_CORE_TYPE   CoreType,
  OUT BOOLEAN                    *IsConnected  
  );


/* QCOM_USB_CONFIG_PROTOCOL_GET_USBHOST_CONFIG */ 
/** @ingroup efi_usbConfig_get_usbhost_config
  @par Summary
  Gets the appropriate USB host core type for each platform.

  @param[in]   This          Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[out]  CoreType      USB core and mode; see #QCOM_USB_CONFIG_CORE_TYPE 
                             for details.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_USBHOST_CONFIG)(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_MODE_TYPE        Mode,
  IN  QCOM_USB_CORE_NUM         CoreNum,
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  );

/* QCOM_USB_CONFIG_PROTOCOL_GET_USB_MAXHOSTCORENUM */ 
/** @ingroup efi_usbConfig_get_usbhost_config
  @par Summary
  Gets maximum number of core(s) supported in host mode for each platform

  @param[in]   This          Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]   Mode          USB Mode
  @param[out]  CoreNum       Maximum number of cores supported in host mode

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_USB_MAXHOSTCORENUM)(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_MODE_TYPE        Mode,
  OUT UINT8                     *CoreNum
  );


/* QCOM_USB_CONFIG_PROTOCOL_GET_VBUS_STATUS */ 
/*Gets the Vbus Status
   This is required by host stack(xhci) to trigger re-enable Vbus if required

  @param[in]   This               Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]   CoreNum            CoreNumber (port) specified to be queries for Vbus status
  @param[out]  VbusStatus         Vbus status (based on enum type VBUS_STATUS_TYPE)

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  Others                -- Failure
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_VBUS_STATUS)(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_CORE_NUM         CoreNum,
  OUT UINT32                    *VbusStatus
  );

/* QCOM_USB_CONFIG_PROTOCOL_ENABLE_VBUS */ 
/* Forcefully enable the Vbus for the core in host mode.
 This would be required in case Vbus was turned off due to some reason

  @param[in]   This          Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]   CoreNum       CoreNumber (port) specified for which Vbus needs to be enabkled

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  Others           -- Failure

*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_ENABLE_VBUS)(
  IN  QCOM_USB_CONFIG_PROTOCOL  *This,
  IN  QCOM_USB_CORE_NUM         CoreNum
  );


/* QCOM_USB_CONFIG_PROTOCOL_EXIT_BOOT_SERVICES */
/* This is required since KD does not have i2c and they will reconfigure GPIOs
   to trigger attach */
/** @ingroup efi_usbConfig_exit_lib_services
  @par Summary
  UsbLib call for exiting from uefi.
  
  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_EXIT_USBLIB_SERVICES)(
  );

/* QCOM_USB_CONFIG_PROTOCOL_ENTER_LPM */ 
/* Configures the core in low power mode
  @par Summary
  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]  Mode           USB mode; see #QCOM_USB_MODE_TYPE for details.
  @param[in]  CoreNum        USB corenum; see #QCOM_USB_NUM for details.

  @return
  EFI_SUCCESS       --      Function completed successfully
  Other             --      Error occurred during the operation

  @Comments: EnterLPM function is invoked in case of dead battery.
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_ENTER_LPM)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  );

/* QCOM_USB_CONFIG_PROTOCOL_EXIT_LPM */ 
/* Configures the core to exit low power mode
  @par Summary
  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
  @param[in]  Mode           USB mode; see #QCOM_USB_MODE_TYPE for details.
  @param[in]  CoreNum        USB corenum; see #QCOM_USB_NUM for details.

  @return
  EFI_SUCCESS       --      Function completed successfully
  Other             --      Error occurred during the operation

  @Comments: ExitLPM function is invoked in case of dead battery when charger is removed and we are about to shutdown
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_EXIT_LPM)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  );


/* QCOM_USB_CONFIG_PROTOCOL_TOGGLE_USB_MODE */ 
/* The function suggests to toggles the USB mode of the specified core. If the
   switch (peripheral to host or host to peripheral) is supported, 
   the core originally configured in peripheral mode will be switch to 
   the host mode and vice versa.

  @par Summary
  @param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.  
  @param[in]  CoreNum        USB corenum; see #QCOM_USB_NUM for details.

  @return
  EFI_SUCCESS       --      Function completed successfully
  Other             --      Error occurred during the operation  
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_TOGGLE_USB_MODE)(
  IN QCOM_USB_CONFIG_PROTOCOL   *This,
  IN QCOM_USB_CORE_NUM          CoreNum
  );


/* QCOM_USB_CONFIG_PROTOCOL_GET_CORE_COUNT */
/* 
@par Summary
The function retrieves the total USB cores in the system

@param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
@param[out] Count          Pointer to the USB core count.

@return
EFI_SUCCESS       --      Function completed successfully
Other             --      Error occurred during the operation
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_CORE_COUNT)(
    IN  QCOM_USB_CONFIG_PROTOCOL   *This,
    OUT UINT8                      *Count
    );


/* QCOM_USB_CONFIG_PROTOCOL_GET_SUPPORTED_MODE */
/* 
@par Summary
Retrieve the supported USB mode on the core

@param[in]  This           Pointer to the QCOM_USB_CONFIG_PROTOCOL instance.
@param[in]  CoreNum        USB corenum; see #QCOM_USB_NUM for details.
@param[out] ModeType       Pointer to the supported ModeType of the core

@return
EFI_SUCCESS       --      Function completed successfully
Other             --      Error occurred during the operation
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_USB_CONFIG_PROTOCOL_GET_SUPPORTED_MODE)(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CORE_NUM          CoreNum, 
  OUT QCOM_USB_MODE_TYPE         *ModeType
  );


/** @addtogroup efi_usbConfig_constants 
@{ */
/** 
  Protocol version.
*/
#define QCOM_USB_CONFIG_PROTOCOL_REVISION  0x00010003
/** @} */ /* end_addtogroup efi_usbConfig_constants */
/** @cond */
#define USB_CONFIG_INTERFACE_REVISION      QCOM_USB_CONFIG_PROTOCOL_REVISION
/** @endcond */

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_usbConfig_protocol
  @par Summary
  Qualcomm USB Configuration Protocol interface.

  @par Parameters
  @inputprotoparams{usb_config_proto_params.tex} 
*/
struct _QCOM_USB_CONFIG_PROTOCOL {
  UINT32                                           	    Revision;
  QCOM_USB_CONFIG_PROTOCOL_GET_CORE_BASE_ADDR      	    GetCoreBaseAddr;
  QCOM_USB_CONFIG_PROTOCOL_CONFIG_USB              	    ConfigUsb;
  QCOM_USB_CONFIG_PROTOCOL_RESET_USB               	    ResetUsb;
  QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONFIG        	    GetUsbFnConfig;
  QCOM_USB_CONFIG_PROTOCOL_GET_SS_USBFN_CONFIG     	    GetSSUsbFnConfig;
  QCOM_USB_CONFIG_PROTOCOL_GET_USBFN_CONN_STATUS        GetUsbFnConnStatus;
  QCOM_USB_CONFIG_PROTOCOL_GET_USBHOST_CONFIG      	    GetUsbHostConfig;
  QCOM_USB_CONFIG_PROTOCOL_GET_USB_MAXHOSTCORENUM     	GetUsbMaxHostCoreNum;
  QCOM_USB_CONFIG_PROTOCOL_EXIT_USBLIB_SERVICES    	    ExitUsbLibServices;
  QCOM_USB_CONFIG_PROTOCOL_START_CONTROLLER           	StartController;
  QCOM_USB_CONFIG_PROTOCOL_STOP_CONTROLLER         	    StopController;
  QCOM_USB_CONFIG_PROTOCOL_ENTER_LPM                   	EnterLPM;
  QCOM_USB_CONFIG_PROTOCOL_EXIT_LPM                	    ExitLPM;
  QCOM_USB_CONFIG_PROTOCOL_TOGGLE_USB_MODE         	    ToggleUsbMode;
  QCOM_USB_CONFIG_PROTOCOL_GET_CORE_COUNT               GetCoreCount;
  QCOM_USB_CONFIG_PROTOCOL_GET_SUPPORTED_MODE           GetSupUsbMode;
  QCOM_USB_CORE_NUM                                	    coreNum;
  QCOM_USB_MODE_TYPE                               	    modeType;
  QCOM_USB_CONFIG_PROTOCOL_GET_VBUS_STATUS              GetUsbVbusStatus;
  QCOM_USB_CONFIG_PROTOCOL_ENABLE_VBUS                  UsbEnableVbus;
};
  
#endif
