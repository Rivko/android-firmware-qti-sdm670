/** 
  @file  EFIDisplayPwrCtrl.h
  @brief UEFI display power protocol definitions.
*/
/*=============================================================================
    Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
    All Rights Reserved
    Qualcomm Confidential and Proprietary
=============================================================================*/

#ifndef __EFIDISPLAYPWRCTRL_H__
#define __EFIDISPLAYPWRCTRL_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_displayPwrCtrl_constants  
@{ */
/**
  Protocol version.
*/
#define DISPLAY_PWR_CTRL_REVISION 0x0000000000010000

/**
  Minimum and maximum backlight power levels percentage
*/
#define DISPLAY_MIN_BACKLIGHT_LEVEL    0
#define DISPLAY_MAX_BACKLIGHT_LEVEL  100

/** @} */ /* end_addtogroup efi_displayPwrCtrl_constants */

/*  Protocol GUID definition */
/** @ingroup efi_displayPwrCtrl_protocol */
#define EFI_DISPLAYPWRCTRL_PROTOCOL_GUID \
    {0x7bfa4293, 0x7aa4, 0x4375, {0xb6, 0x3c, 0xb6, 0xaa, 0xb7, 0x86, 0xc4, 0x3c}}

// {68C3B70D-0F96-4006-99F6-0FD211F5F2B2}
#define EFI_UI_ACTIVE_EVENT_GUID \
    { 0x68c3b70d, 0xf96, 0x4006, { 0x99, 0xf6, 0xf, 0xd2, 0x11, 0xf5, 0xf2, 0xb2 } };

// {30D39942-9556-4887-A943-BC63805D9D43}
#define EFI_UI_IDLE_EVENT_GUID \
    {0x30d39942, 0x9556, 0x4887, { 0xa9, 0x43, 0xbc, 0x63, 0x80, 0x5d, 0x9d, 0x43 }};

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the Display Power Control Protocol GUID.
*/
extern EFI_GUID gQcomDisplayPwrCtrlProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL;
/** @endcond */

/** @addtogroup efi_displayPwrCtrl_data_types 
@{ */
/** 
  Display type.
*/
typedef enum
{
  EFI_DISPLAY_TYPE_PRIMARY = 0,    /**< Primary display    */  
  EFI_DISPLAY_TYPE_EXTERNAL,       /**< External display   */
  EFI_DISPLAY_TYPE_ALL,            /**< Primary and External displays */
  EFI_DISPLAY_TYPE_MAX
}EFI_DISPLAY_TYPE;


/** 
  Display state. 
*/
typedef enum
{
  EFI_DISPLAY_POWER_STATE_OFF = 0,   /**< Display off state  */
  EFI_DISPLAY_POWER_STATE_ON,        /**< Display on state */
  EFI_DISPLAY_POWER_STATE_MAX     
}EFI_DISPLAY_POWER_CTRL_STATE;
/** @} */ /* end_addtogroup efi_displayPwrCtrl_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_DISPLAY_PANEL_POWER_CONTROL */ 
/** @ingroup efi_displaypanel_power_control
  @par Summary
  Sets the display panel power state 

  @param[in] DisplayType         Primary display     : EFI_DISPLAY_TYPE_PRIMARY
                                 Secondary display   : EFI_DISPLAY_TYPE_SECONDARY
                                 External display    : EFI_DISPLAY_TYPE_EXTERNAL
  @param[in] DisplayState        ON  : EFI_DISPLAY_POWER_STATE_ON   
                                 OFF : EFI_DISPLAY_POWER_STATE_OFF
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_PANEL_POWER_CONTROL)(
  IN EFI_DISPLAY_TYPE                DisplayType,
  IN EFI_DISPLAY_POWER_CTRL_STATE    DisplayState
);


/* EFI_DISPLAY_PANEL_POWER_STATUS */ 
/** @ingroup efi_displaypanel_power_status
  @par Summary
  Retrieves the display panel power state 

  @param[in] DisplayType         Primary display     : EFI_DISPLAY_TYPE_PRIMARY
                                 Secondary display   : EFI_DISPLAY_TYPE_SECONDARY
                                 External display    : EFI_DISPLAY_TYPE_EXTERNAL
  @param[out] DisplayState       ON  : EFI_DISPLAY_POWER_STATE_ON   
                                 OFF : EFI_DISPLAY_POWER_STATE_OFF
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.    
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_PANEL_POWER_STATUS)(
  IN EFI_DISPLAY_TYPE                DisplayType,
  IN EFI_DISPLAY_POWER_CTRL_STATE   *DisplayState
);


/* EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_CONTROL */ 
/** @ingroup efi_displaybacklight_brightness_level_control
  @par Summary
  Sets the display backlight brightness level

  @param[in] DisplayType          Primary display     : EFI_DISPLAY_TYPE_PRIMARY
                                  Secondary display   : EFI_DISPLAY_TYPE_SECONDARY
                                  External display    : EFI_DISPLAY_TYPE_EXTERNAL
  @param[in] BrightnessLevel      0% to 100%                                 
  
  @return
  EFI_SUCCESS               : Function returned successfully.
  EFI_INVALID_PARAMETER     : Parameter passed is incorrect.  
  EFI_UNSUPPORTED           : Parameter passed is not supported. 
  EFI_DEVICE_ERROR          : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_CONTROL)(
  IN EFI_DISPLAY_TYPE          DisplayType,
  IN UINT32                    BrightnessLevel
);


/* EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_STATUS */ 
/** @ingroup efi_displaybacklight_brightness_level_status
  @par Summary
  Retrieves the display backlight brightness level

  @param[in] DisplayType           Primary display     : EFI_DISPLAY_TYPE_PRIMARY
                                   Secondary display   : EFI_DISPLAY_TYPE_SECONDARY 
                                   External display    : EFI_DISPLAY_TYPE_EXTERNAL                                                
  @param[out] BrightnessLevel      0% to 100%                                 
  
  @return
  EFI_SUCCESS           : Function returned successfully. 
  EFI_UNSUPPORTED       : Parameter passed is not supported. 
  EFI_DEVICE_ERROR      : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_STATUS)(
  IN EFI_DISPLAY_TYPE         DisplayType,
  IN UINT32                  *BrightnessLevel
);

//Sets the display panel power state 
EFI_STATUS
EFIAPI
EFI_DisplayPanelPowerControl
(
IN EFI_DISPLAY_TYPE                DisplayType,
IN EFI_DISPLAY_POWER_CTRL_STATE    DisplayState
);
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_displayPwrCtrl_protocol
  @par Summary
  Qualcomm Display Power Control Protocol interface.

  @par Parameters
  @inputprotoparams{} 
*/
struct _EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL
{
  UINT64                                           Revision;
  EFI_DISPLAY_PANEL_POWER_CONTROL                  DisplayPanelPowerControl; 
  EFI_DISPLAY_PANEL_POWER_STATUS                   DisplayPanelPowerStatus;
  EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_CONTROL   DisplayBacklightBrightnessLevelControl;
  EFI_DISPLAY_BACKLIGHT_BRIGHTNESS_LEVEL_STATUS    DisplayBacklightBrightnessLevelStatus;
};

#endif  /* __EFIDISPLAYPWRCTRL_H__ */
