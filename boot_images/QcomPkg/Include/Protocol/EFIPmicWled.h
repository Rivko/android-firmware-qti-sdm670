/** @file EFIPmicWled.h

  PMIC WLED Protocol definitions

  Copyright (c) 2012 - 2017, Qualcomm Technologies Inc. All 
  rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/19/16   al      Adding APIs for configuring SWIRE default value
 03/29/16   al      Updating for newer target
 28/05/14   al      Adding APIs for configuring various AMOLED param.
 06/23/14   al      Added GetStatus 
 06/12/14   al      Updating as per new target
 04/29/14   al      Added API to enable AMOLED 
 08/28/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/14/13   al      Replacing enums with type uint32 
 12/21/12   al      Updating eumn for SWITCHING FREQUENCY  
 08/16/12   al      Changed EFI_PM_WLED_BST_MAX_DUTY_TYPE as per 8974 WLED document
 07/27/12   al      Renamed all Enums,added device index and resource index
 03/05/12   sm      Changes parameters for EFI_PM_WLED_ENABLE_SYNC
 02/08/12   sm      New file.
=============================================================================*/

#ifndef __EFIPMICWLED_H__
#define __EFIPMICWLED_H__

/** @ingroup efi_pmicWled_constants */
/** Protocol version. */
#define PMIC_WLED_REVISION 0x0000000000010009

/* Protocol GUID definition */
/** @ingroup efi_pmicWled_protocol */
#define EFI_PMIC_WLED_PROTOCOL_GUID \
{ 0x5bef7c41, 0xa560, 0x435b, { 0x83, 0x5b, 0x3c, 0x3e, 0xbf, 0x2b, 0xd1, 0x51 } }


/* PMIC WLED UEFI typedefs */
/** @cond */
typedef struct _EFI_QCOM_PMIC_WLED_PROTOCOL   EFI_QCOM_PMIC_WLED_PROTOCOL;
/** @endcond */

/* PMIC UEFI WLED Procotol GUID */
/** @ingroup efi_pmicWled_protocol */
extern EFI_GUID gQcomPmicWledProtocolGuid;

/** @addtogroup efi_pmicWled_data_types 
@{ 
  LED type
 */
typedef enum {
  EFI_PM_WLED_NONE = 0,       /**< None. */
  EFI_PM_WLED_LED1 = 1,       /**< LED 1. */
  EFI_PM_WLED_LED2 = 2,       /**< LED 2. */
  EFI_PM_WLED_LED3 = 4,       /**< LED 3. */
  EFI_PM_WLED_LED4 = 8,
  EFI_PM_WLED_ALL = (EFI_PM_WLED_LED1 | EFI_PM_WLED_LED2 | EFI_PM_WLED_LED3 | EFI_PM_WLED_LED4), /**< All LEDs. */
  EFI_PM_WLED_INVALID     /**< Invalid value. */
} EFI_PM_WLED_LED_TYPE;

typedef enum
{
  EFI_PM_WLED_DISPLAY_TYPE_LCD,
  EFI_PM_WLED_DISPLAY_TYPE_AMOLED,
  EFI_PM_WLED_DISPLAY_TYPE_INVALID,
}EFI_PM_WLED_DISPLAY_TYPE;

/**
  Pulse-width modulation type.
 */
typedef enum {
  EFI_PM_WLED_PWM_1,  /**< PWM 1. */
  EFI_PM_WLED_PWM_2,  /**< PWM 2. */
  EFI_PM_WLED_PWM_3,  /**< PWM 3. */
  EFI_PM_WLED_PWM_INVALID  /**< Invalid value. */
} EFI_PM_WLED_PWM_TYPE;

/**
  Internal digital modulator type.
 */
typedef enum {
  EFI_PM_WLED_MODE_12b_SDM = 0,  /**< 12b sigma-delta modulation. */
  EFI_PM_WLED_MODE_9b_PWM,       /**< 9b pulse-width modulation. */
  EFI_PM_WLED_MODE_INVALID       /**< Invalid value. */
} EFI_PM_WLED_PWM_MODE_TYPE;

/**
  Maximum duty cycle control for boost.
 */
typedef enum {
  EFI_PM_WLED_BST_MAX_DUTY_SUB_156 =0,   /**< (1/Fsw - 156ns)*Fsw. */ 
  EFI_PM_WLED_BST_MAX_DUTY_SUB_104,      /**< (1/Fsw - 104ns)*Fsw. */
  EFI_PM_WLED_BST_MAX_DUTY_SUB_52,       /**< (1/Fsw - 52ns)*Fsw. */
  EFI_PM_WLED_BST_MAX_DUTY_SUB_26,       /**< (1/Fsw - 26ns)*Fsw. */	
  EFI_PM_WLED_BST_MAX_DUTY_SUB_INVALID   /**< Invalid value. */
} EFI_PM_WLED_BST_MAX_DUTY_TYPE;

/**
  Digital modulator input clock frequency control value.
 */
typedef enum {
  EFI_PM_WLED_MOD_CLK_19P2MHz = 0,  /**< 19.2 MHz. */
  EFI_PM_WLED_MOD_CLK_4P8MHz,       /**< 4.8 MHz. */
  EFI_PM_WLED_MOD_CLK_2P4MHz,       /**< 2.4 MHz. */
  EFI_PM_WLED_MOD_CLK_1P2MHz,       /**< 1.2 MHz. */
  EFI_PM_WLED_MOD_CLK_INVALID       /**< Invalid value. */
} EFI_PM_WLED_MOD_CLK_TYPE;

/**
  Modulator dimming mode control type.
 */
typedef enum {
  EFI_PM_WLED_MOD_DIM_MODE_ANALOG = 0,  /**< Analog. */
  EFI_PM_WLED_MOD_DIM_MODE_DIGITAL,     /**< Digital. */
  EFI_PM_WLED_MOD_DIM_MODE_INVALID      /**< Invalid value. */
} EFI_PM_WLED_MOD_DIM_MODE_TYPE;

/**
  Warm-up time delay value.
 */
typedef enum {
  EFI_PM_WLED_WARMUP_DELAY_6P7US,   /**< 6.7 microsecond. */
  EFI_PM_WLED_WARMUP_DELAY_13P3US,  /**< 13.3 microsecond. */
  EFI_PM_WLED_WARMUP_DELAY_26P7US,  /**< 26.7 microsecond. */
  EFI_PM_WLED_WARMUP_DELAY_53P4US,  /**< 53.4 microsecond. */
  EFI_PM_WLED_WARMUP_DELAY_INVALID  /**< Invalid value. @newpage */
} EFI_PM_WLED_WARMUP_DLY_TYPE;

/**
  Overvoltage protection threshold value.
 */
typedef enum {
  EFI_PM_WLED_OVP_THR_31V=0,         //31V
  EFI_PM_WLED_OVP_THR_29P6V,         //29.6V
  EFI_PM_WLED_OVP_THR_19P6V,         //19.6V
  EFI_PM_WLED_OVP_THR_17P8V,         //17.8V
  EFI_PM_WLED_OVP_THR_18P1V = EFI_PM_WLED_OVP_THR_17P8V,  //18.1V
  EFI_PM_WLED_OVP_THR_7P84V=0,       //7.84V
  EFI_PM_WLED_OVP_THR_7P65V,          //7.65V
  EFI_PM_WLED_OVP_THR_6P11V,         //6.11V
  EFI_PM_WLED_OVP_THR_5P80V,         //5.8V
  EFI_PM_WLED_OVP_THR_INVALID       /**< Invalid value. */
} EFI_PM_WLED_OVP_THRESHOLD_TYPE;

/**
  Value of the boost current limit.
 */
typedef enum {
  EFI_PM_WLED_BST_ILIMIT_105mA = 0,  /**< 105 mA. */
  EFI_PM_WLED_BST_ILIMIT_280mA,      /**< 280 mA. */
  EFI_PM_WLED_BST_ILIMIT_525mA,      /**< 525 mA. */
  EFI_PM_WLED_BST_ILIMIT_450mA = EFI_PM_WLED_BST_ILIMIT_525mA,    /**< 450 mA. */
  EFI_PM_WLED_BST_ILIMIT_620mA,      /**< 620 mA. */
  EFI_PM_WLED_BST_ILIMIT_970mA,      /**< 970 mA. */
  EFI_PM_WLED_BST_ILIMIT_1150mA,     /**< 1150 mA. */
  EFI_PM_WLED_BST_ILIMIT_1300mA,     /**< 1300 mA. */
  EFI_PM_WLED_BST_ILIMIT_1500mA,     /**< 1500 mA. */
  EFI_PM_WLED_BST_ILIMIT_INVALID     /**< Invalid value. */
} EFI_PM_WLED_BST_ILIMIT_TYPE;

/**
  Slew rate value.
 */
typedef enum {
  EFI_PM_WLED_SLEW_RATE_20nS = 0,  /**< 20 ns. */
  EFI_PM_WLED_SLEW_RATE_15nS,      /**< 15 ns. */
  EFI_PM_WLED_SLEW_RATE_10nS,      /**< 10 ns. */
  EFI_PM_WLED_SLEW_RATE_5nS,       /**< 5 ns. */
  EFI_PM_WLED_SLEW_RATE_1nS=0,      //1ns 
  EFI_PM_WLED_SLEW_RATE_1p5nS,      //1p5nS
  EFI_PM_WLED_SLEW_RATE_2nS,        //2nS
  EFI_PM_WLED_SLEW_RATE_2p5nS,      //2p5nS
  EFI_PM_WLED_SLEW_RATE_INVALID    /**< Invalid value. */
} EFI_PM_WLED_SLEW_RATE_TYPE;


/* 
Debounce time for short detection happened during normal operation
*/
typedef enum {
  EFI_PM_WLED_DBNC_SHORT_TIME_2_BY_FSW = 0, //(2/Fsw)
  EFI_PM_WLED_DBNC_SHORT_TIME_4_BY_FSW,     //(4/Fsw)
  EFI_PM_WLED_DBNC_SHORT_TIME_8_BY_FSW,     //(8/Fsw)
  EFI_PM_WLED_DBNC_SHORT_TIME_16_BY_FSW,    //(16/Fsw)
  EFI_PM_WLED_DBNC_SHORT_TIME_INVALID
} EFI_PM_WLED_DBNC_SHORT_TIME_TYPE;


typedef struct 
{
  BOOLEAN                           ShortCktAlertToPbs;     // TRUE :send an alert to PBS; FALSE: use external PFET when short circuit condition is detected   
  EFI_PM_WLED_DBNC_SHORT_TIME_TYPE  DbncTimeShortCkt;       //debounce time for short detection                                                                
  BOOLEAN                           EnDischargeResistance;  //connect/disconnect discharge resistance                                                          
  BOOLEAN                           EnShortCkt;             //enable/disable short protection                                                                  
}EFI_PM_WLED_SHORT_CKT_CONFIG_TYPE;


typedef enum
{
  EFI_PM_WLED_MASK_ERRAMP_EN_MASK = 0,
  /* error amplifier controlled by module enable signals */
  EFI_PM_WLED_MASK_ILIM_EN_MASK,
  /* current limit controlled by module enable signals */
  EFI_PM_WLED_MASK_VREF_EN_MASK,
  /* reference controlled by module VREF enable signals */
  EFI_PM_WLED_MASK_CS_REF_EN_MASK,
  /* reference controlled by module CS enable signals */
  EFI_PM_WLED_MASK_SLOPE_COMP_EN_MASK,
  /* slope compensation controlled by module enable signals */
  EFI_PM_WLED_MASK_OVP_EN_MASK,
  /* OVP is controlled by module enable signals */
  EFI_PM_WLED_MASK_T_ERRAMP_OUT,
  /*  enables monitoring error amp output w/ loop filter disconnected */
  EFI_PM_WLED_MASK_RDSON_TM,
  /* NFET is turned completely on. Test mode for measuring Rdson of integrated NFET */
  EFI_PM_WLED_MASK_INVALID,
  /* Invalid */
}EFI_PM_WLED_MASK_TYPE;


typedef enum
{
  EFI_PM_WLED_SH_CONFIG_ENB_IIND_UP =0,
  /*Early turn on the boost NFET when ilim happens */
  EFI_PM_WLED_SH_CONFIG_EN_SOFT_START,
  /*Enable soft start Ilim ramp after WLED module enable */
  EFI_PM_WLED_SH_CONFIG_EN_SLEEP_CLK_REQUEST,
   /*WLED module request for 19.2MHz clock is disabled when sleep_b = 1'b0 */
  EFI_PM_WLED_SH_CONFIG_DEBOUNCE_BYPASS,
  /*debouncing is removed for Ilim and OVP circuits */
  EFI_PM_WLED_SH_CONFIG_EN_CLAMP,
  /*Enable to clamp the error amp output */
  EFI_PM_WLED_SH_CONFIG_EN_VREF_UP,         
  /*Enable S/H during soft start*/
  EFI_PM_WLED_SH_CONFIG_DEBOUNCE_BYPASS_ILIM, 
  /*Enable S/H Debounce for Ilim circuits*/
  EFI_PM_WLED_SH_CONFIG_EN_SH_FOR_MIN_SEL,    
  /*Enable S/H in the minimum selector path*/
  EFI_PM_WLED_SH_CONFIG_INVALID,
   /*Invalid */
}EFI_PM_WLED_SH_CONFIG_TYPE;


typedef enum
{
  EFI_PM_WLED_AMOLED_CTRL_TYPE_SWIRE,
  /*AMOLED control is S-Wire*/
  EFI_PM_WLED_AMOLED_CTRL_TYPE_SPMI,
  /*AMOLED control is SPMI*/
  EFI_PM_WLED_AMOLED_CTRL_TYPE_INVALID,
}EFI_PM_WLED_AMOLED_CTRL_TYPE;


/** @} */ /* end_addtogroup efi_pmicWled_data_types */ 
typedef struct
{
   BOOLEAN                         Led1ModulatorEn;
   BOOLEAN                         Led2ModulatorEn;
   BOOLEAN                         Led3ModulatorEn;
   BOOLEAN                         Led4ModulatorEn;
                                   
   BOOLEAN                         Led1CurrentSinkEn;
   BOOLEAN                         Led2CurrentSinkEn;
   BOOLEAN                         Led3CurrentSinkEn;
   BOOLEAN                         led4CurrentSinkEn;

   UINT32                          SlopeTrim;

   EFI_PM_WLED_MOD_CLK_TYPE        ModeClk; 
   BOOLEAN                         EnPhaseStagger;
   EFI_PM_WLED_MOD_CLK_TYPE        AccumClk;
   BOOLEAN                         EnHybridDim;
   EFI_PM_WLED_PWM_MODE_TYPE       PwmMode;
   EFI_PM_WLED_MOD_DIM_MODE_TYPE   DimMode;
   EFI_PM_WLED_BST_MAX_DUTY_TYPE   BstMaxDuty;
   UINT32                          SetFswCtrl;
   UINT32                          RampTime;
   EFI_PM_WLED_BST_ILIMIT_TYPE     BstIlimit;
   EFI_PM_WLED_SLEW_RATE_TYPE      SlewRate;
   UINT32                          VrefCtrl;
   EFI_PM_WLED_WARMUP_DLY_TYPE     WarmupDly;
   EFI_PM_WLED_OVP_THRESHOLD_TYPE  OvpThr;
   UINT32                          Led1FullScaleCurrent;
   UINT32                          Led2FullScaleCurrent;
   UINT32                          Led3FullScaleCurrent;
   UINT32                          Led4FullScaleCurrent;
   UINT32                          Led1Brightness;
   UINT32                          Led2Brightness;
   UINT32                          Led3Brightness;
   UINT32                          Led4Brightness;
}EFI_PM_WLED_STATUS_TYPE;
/** @ingroup efi_pm_wled_enable
  @par Summary
  Enables the WLED module. 

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ResourceIndex    Resource index. 
  @param[in]  Enable           Whether or not the WLED module is enabled.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE)(
  IN UINT32  PmicDeviceIndex, 
  IN UINT32  ResourceIndex,
  IN BOOLEAN Enable
);

/* EFI_PM_WLED_ENABLE_MODULATOR */
/** @ingroup efi_pm_wled_enable_modulator
  @par Summary
  Enables the digital modulator generator and modulator input for each current 
  sink.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType          LED type; see #EFI_PM_WLED_LED_TYPE for details.
  @param[in]  Enable           To enable TRUE and to disable FALS.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE_MODULATOR)(
  IN UINT32                PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE  LedType,
  IN BOOLEAN               Enable
);

/* EFI_PM_WLED_ENABLE_CURRENT_SINK */
/** @ingroup efi_pm_wled_enable_current_sink
  @par Summary
  Enables the control for the LED current sink outputs.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType          LED type; see #EFI_PM_WLED_LED_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE_CURRENT_SINK)(
  IN UINT32 PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE   LedType
);


/* EFI_PM_WLED_SET_CURRENT_SINK */
/** @ingroup efi_pm_wled_set_current_sink
  @par Summary
  Sets the LED current sink number. 
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  IledType         ILED type; see #EFI_PM_WLED_ILED_TYPE for 
                               details.
  @param[in]  CurrentMilliAmp  LED current sink number in milliamps.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_CURRENT_SINK)(
  IN UINT32                  PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE    LedType,
  IN UINT32                  CurrentMilliAmp
);

/* EFI_PM_WLED_ENABLE_EXT_PWM */
/** @ingroup efi_pm_wled_enable_ext_pwm
  @par Summary
  Enables using extended PWM as the PWM dimming control.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  PwmType          Pulse-width modulation type; see 
                               #EFI_PM_WLED_PWM_TYPE for details.
  @param[in]  Enable           If TRUE, enable extended PWM; if FALSE, disable 
                               extended PWM.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE_EXT_PWM)(
  IN UINT32                 PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE   LedType,
  IN BOOLEAN                Enable
);


/* EFI_PM_WLED_SET_LED_DUTY_CYCLE */
/** @ingroup efi_pm_wled_set_led_duty_cycle
  @par Summary
  Sets the 12-bit duty cycle control for the indicated LED digital modulator 
  control.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType          LED type; see #EFI_PM_WLED_LED_TYPE for details. 
  @param[in]  Data             Data is between 0x000 to 0xFFF. This 12-bit data 
                               is used to set the current value for the LED 
                               duty cycle. The final current for the duty 
                               cycle = 12-bit data * 6.1 microampere.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_LED_DUTY_CYCLE)(
  IN UINT32                  PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE    LedType,
  IN UINT16                  Data
);


/* EFI_PM_WLED_ENABLE_CABC */
/** @ingroup efi_pm_wled_enable_cabc
  @par Summary
  Enables the content adaptive backlight control.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType          LED type; see #EFI_PM_WLED_LED_TYPE for details. 
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE_CABC)(
  IN UINT32                PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE  LedType
);

/* EFI_PM_WLED_ENABLE_SYNC */
/** @ingroup efi_pm_wled_enable_sync
  @par Summary
  Enables the LED sync. This updates the 12-bit register values to 
  the modulator and the full scale current setting register.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType          LED type; see #EFI_PM_WLED_LED_TYPE for details. 
  @param[in]  Enable           Whether or not the LED sync is enabled.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_ENABLE_SYNC)(
  IN UINT32                  PmicDeviceIndex,
  IN EFI_PM_WLED_LED_TYPE    LedType,
  IN BOOLEAN                 Enable
);


/* EFI_PM_WLED_SET_FSW_CTRL */
/** @ingroup efi_pm_wled_set_fsw_ctrl
  @par Summary
  Selects the boost switching frequency control.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ResourceIndex    Resource index. 
  @param[in]  FswKhz           Boost switching frequency control in kilohertz.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_FSW_CTRL)(
  IN UINT32 PmicDeviceIndex, 
  IN UINT32 ResourceIndex,
  IN UINT32 FswKhz
);


/* EFI_PM_WLED_SET_OVP_THRESHOLD */
/** @ingroup efi_pm_wled_set_ovp_threshold
  @par Summary
  Selects the overvoltage protection threshold.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ResourceIndex    Resource index. 
  @param[in]  OvpThrType       Overvoltage protection threshold; see 
                               #EFI_PM_WLED_OVP_THRESHOLD_TYPE

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_OVP_THRESHOLD)(
  IN UINT32                            PmicDeviceIndex, 
  IN UINT32                            ResourceIndex,
  IN EFI_PM_WLED_OVP_THRESHOLD_TYPE    OvpThrType
);


/* EFI_PM_WLED_SET_BOOST_ILIMIT */
/** @ingroup efi_pm_wled_set_boost_ilimit
  @par Summary
  Selects the boost current limit.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ResourceIndex    Resource index. 
  @param[in]  BstIlimitType    Boost current limit; see 
                               #EFI_PM_WLED_BST_ILIMIT_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_BOOST_ILIMIT)(
  IN UINT32                       PmicDeviceIndex, 
  IN UINT32                       ResourceIndex,
  IN EFI_PM_WLED_BST_ILIMIT_TYPE  BstIlimitType
);


/* EFI_PM_WLED_SELECT_SLEW_RATE */
/** @ingroup efi_pm_wled_select_slew_rate
  @par Summary
  Selects the slew rate.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ResourceIndex    Resource index. 
  @param[in]  SlewRateType     Slew rate; see #EFI_PM_WLED_SLEW_RATE_TYPE for 
                               details. 

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SELECT_SLEW_RATE)(
  IN UINT32                      PmicDeviceIndex, 
  IN UINT32                      ResourceIndex,
  IN EFI_PM_WLED_SLEW_RATE_TYPE  SlewRateType
);

/* EFI_PM_WLED_SET_AMOLED_VOUT */
/** @ingroup efi_pm_wled_set_amoled_vout
  @par Summary
  Selects the slew rate.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  VoutCtrl         Vout control S-Wire or SPMI. See #EFI_PM_WLED_AMOLED_CTRL_TYPE for more info
  @param[in]  AvddMilliVolt    AvddMilliVolt. Valid Range is  5650 to 7900 milli volt

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_AMOLED_VOUT)(
  IN UINT32                       PmicDeviceIndex,
   IN EFI_PM_WLED_AMOLED_CTRL_TYPE VoutCtrl,
   IN UINT32                       AvddMilliVolt
);


/* EFI_PM_WLED_SELECT_COMP_CAP */
/** @ingroup efi_pm_wled_select_comp_cap
  @par Summary
  Sets the compensator capacitance.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  CzPicoFarad      Compensator capacitance in picofarads.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SELECT_COMP_CAP)(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 CzPicoFarad
);


/**
 * @brief This function enables/disables WLED AMOLED
 * 
 * @param[in] PmicDeviceIndex: PMIC device index. Primary 0, Secondary 1
 * @param[in] Enable         : TRUE to enable and FALSE to disable
 *
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_EN_AMOLED)(
  IN UINT32 PmicDeviceIndex, 
  IN BOOLEAN Enable
);


/**
 * @brief This function reads the WLED status
 * 
 * @param[in]  PmicDeviceIndex : PMIC device index. 0 for primary and 1 for secondary
 * @param[out] WledStatus      : Wled status.  Refer enum #EFI_PM_WLED_STATUS_TYPE for more info
 * 
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_GET_STATUS)(
  IN  UINT32                  PmicDeviceIndex,
  OUT EFI_PM_WLED_STATUS_TYPE *WledStatus
);


 /**
 * @brief This function sets various dtest for WLED
 * 
 * @param[in] PmicDeviceIndex   : PMIC device index. 0 for primary and 1 for secondary
 * @param[in] DtestSetting  : Various dtest mapping for WLED. Following are options:
 *                            0 : DTEST1, DTEST2 output disabled
 *                            1 : DTEST1 = Gate drive output, DTEST2 disabled
 *                            2 : DTEST1 = OVP comparator for trim, DTEST2 disabled
 *                            3 : DTEST1 = Ilim comparator (ilim_out), DTEST2 = ilim_no_blnk (no blank of ilim)
 *                            4 : DTEST1 = state[0], DTEST2 = state[1]
 *                            5 : DTEST1 = state[2], DTEST2 = state[3]
 *                            6 : DTEST1 = EN_BST, DTEST2 = EN_DSCHGR
 *                            7 : DTEST1 = errAmp_out for trim, DTEST2 disabled
 *                            8 : DTEST1 = psm_skipb(PSM comparator output), DTEST2 = PFM_ilim_rstb
 *                            9 : DTEST1 = Output short detection (DET_SC), DTEST2 = EXT_FET
 *                            10 : DTEST1 = fsw_clk_in; DTEST2 = Max_duty;
 *                            11 : N/A
 *                            12 : N/A
 *                            13 : DTEST1 = sstart[0], DTEST2 = sstart[1]
 *                            14 : DTEST1 = pwr_stg_en, DTEST2 = SS_DONE
 *                            15 : DTEST1 = cabc_sync, DTEST2 output disabled
 *                            16 : DTEST1 = pwm_out_dig<1>, DTEST2 = pwm_out_ana<1>
 *                            17 : DTEST1 = pwm_out_dig<2>, DTEST2 = pwm_out_ana<2>
 *                            18 : DTEST1 = pwm_out_dig<3>, DTEST2 = pwm_out_ana<3>
 *                            19 : DTEST1 = pwm_out_dig<4>, DTEST2 = pwm_out_ana<4>
 *                            20 : DTEST1 = CS_gatedrv<1>, DTEST2 = CS_gatedrv<2>
 *                            21: DTEST1 = CS_gatedrv<3>, DTEST2 = CS_gatedrv<4>
 *                            22 : DTEST1 = otaout_dft<1>, DTEST2 = otaout_dft<2>
 *                            23: DTEST1 = otaout_dft<3>, DTEST2 = otaout_dft<4>
 *                            24 : DTEST1 = pwm_out_dig<1>, DTEST2 = pwm_hv_gate_pull<1>
 *                            25 : DTEST1 = pwm_out_dig<2>, DTEST2 = pwm_hv_gate_pull<2>
 *                            26 : DTEST1 = pwm_out_dig<3>, DTEST2 = pwm_hv_gate_pull<3>
 *                            27 : DTEST1 = pwm_out_dig<4>, DTEST2 = pwm_hv_gate_pull<4>
 *                            28 : DTEST1: cs_hybrid_dim_cmp_outb <1>, DTEST2: cs_hybrid_dim_cmp_outb <2>
 *                            29 : DTEST1: cs_hybrid_dim_cmp_outb <3>, DTEST2: cs_hybrid_dim_cmp_outb <4>
 * 
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_WLED_DTEST)(
    IN UINT32 PmicDeviceIndex, 
    IN UINT32 DtestSetting
    );

/* EFI_PM_WLED_SET_INERT_SWIRE_VOLT */
/** @ingroup efi_pm_wled_set_inert_swire_volt
  @par Summary
  Selects the slew rate.
 
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  AvddMilliVolt    AvddMilliVolt. Valid Range is  5650 to 7900 milli volt

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef  
EFI_STATUS (EFIAPI *EFI_PM_WLED_SET_DORMANT_SWIRE_VOLT)(
    IN UINT32 PmicDeviceIndex,
    IN UINT32 AvddMilliVolt
);

/**
  @brief
  PMIC VREG UEFI Protocol definition
 */
struct _EFI_QCOM_PMIC_WLED_PROTOCOL {
  UINT64                                  Revision;
  EFI_PM_WLED_ENABLE                      EnableWled;
  EFI_PM_WLED_ENABLE_MODULATOR            EnableModulator;
  EFI_PM_WLED_ENABLE_CURRENT_SINK         EnableCurrentSink;
  EFI_PM_WLED_SET_CURRENT_SINK            SetCurrentSink;
  EFI_PM_WLED_ENABLE_EXT_PWM              EnableExtPwm;
  EFI_PM_WLED_SET_LED_DUTY_CYCLE          SetLedDutyCycle;
  EFI_PM_WLED_ENABLE_CABC                 EnableCabc;
  EFI_PM_WLED_ENABLE_SYNC                 EnableSync;
  EFI_PM_WLED_SET_FSW_CTRL                SetFswCtrl;
  EFI_PM_WLED_SET_OVP_THRESHOLD           SetOvpThreshold;
  EFI_PM_WLED_SET_BOOST_ILIMIT            SetBoostIlimit;
  EFI_PM_WLED_SELECT_SLEW_RATE            SelectSlewRate;
  EFI_PM_WLED_SET_AMOLED_VOUT             SetAmoledVout;
  EFI_PM_WLED_EN_AMOLED                   EnAmoled;
  EFI_PM_WLED_GET_STATUS                  GetStatus;
  EFI_PM_WLED_SET_WLED_DTEST              SetWledDtest;
  EFI_PM_WLED_SET_DORMANT_SWIRE_VOLT      SetDormantSwireVolt;
};

#endif  /* __EFIPMICWLED_H__ */
