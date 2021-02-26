/** @ingroup 
@file  EFIPmicIbb.h
@brief PMIC IBB for UEFI.
*/

/*===========================================================================

Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Confidential and Proprietary.
    
===========================================================================*/

/*===========================================================================

EDIT HISTORY

when       who     what, where, why
--------    ---     ---------------------------------------------------------
10/03/17   ks      Added support for new IBB Type/Subtype
12/09/14   al      Sync with latest
06/24/14   al      Added API for configuring IBB delay 
06/09/14   al      Arch update  
05/14/14   al      New file.
===========================================================================*/

#ifndef __EFIPMICIBB_H__
#define __EFIPMICIBB_H__


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
/** @ingroup @addtogroup efi_pmicIbb_constants 
@{ */
/** @ingroup
Protocol version.
*/

#define PMIC_IBB_REVISION 0x0000000000010004
/** @ingroup @} */ /* end_addtogroup efi_pmicIbb_constants */

/*  Protocol GUID definition */
/** @ingroup @ingroup efi_pmicIbb_protocol */
#define EFI_PMIC_IBB_PROTOCOL_GUID \
    { 0xc1bff82d, 0x6c92, 0x48fb, { 0x83, 0x55, 0xc7, 0xfb, 0x42, 0x18, 0xa4, 0xea } }

/** @ingroup @cond */


/*===========================================================================

EXTERNAL VARIABLES

===========================================================================*/
/** @ingroup
External reference to the PMIC IBB Protocol GUID.
*/
extern EFI_GUID gQcomPmicIbbProtocolGuid;


/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
/** @ingroup
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_IBB_PROTOCOL   EFI_QCOM_PMIC_IBB_PROTOCOL;
/** @ingroup @endcond */

/** @ingroup @addtogroup efi_pmicIbb_data_types 
@{ */


typedef enum
{
   EFI_PM_IBB_PWRUP_DLY_SRC_BIAS_ACTIVE, /**< IBB power stage delay source is after IBB's bias active                                        */
   EFI_PM_IBB_PWRUP_DLY_SRC_LAB_VREG_OK, /**< IBB power stage delay source is after VREG_OK from LAB and IBB's bias active 0x0 : BIAS_ACTIVE */
   EFI_PM_IBB_PWRUP_DLY_SRC_INVALID      /**< Invalid                                                                                        */
}EFI_PM_IBB_PWRUP_DLY_SRC_TYPE;

typedef enum
{
   EFI_PM_IBB_PWRDN_DLY_SRC_IBB_DISABLE,   /**< LAB power-down delay source is after IBB disable */
   EFI_PM_IBB_PWRDN_DLY_SRC_IBB_DISCHARGE, /**< LAB power-down delay is after IBB's discharged   */
   EFI_PM_IBB_PWRDN_DLY_SRC_INVALID        /**< Invalid                                          */
}EFI_PM_IBB_PWRDN_DLY_SRC_TYPE;


typedef struct
{
   UINT32                        PwrdnDelayMs;   /**< Delay between IBB and LAB when powering down. Valid values are 1,2,4,8 mS  */
   EFI_PM_IBB_PWRDN_DLY_SRC_TYPE PwrdnDelaySrc;  /**< refer EFI_PM_IBB_PWRDN_DLY_SRC_TYPE for more info*/
   UINT32                        PwrupDelayMs;   /**< Delay between IBB and LAB when powering up. Valid values are 1,2,4,8 mS  */
   BOOLEAN                       EnPwrupDelay;   /**< enable disable powerup delay                      */
   EFI_PM_IBB_PWRUP_DLY_SRC_TYPE PwrupDelaySrc;  /**< refer EFI_PM_IBB_PWRUP_DLY_SRC_TYPE for more info */
} EFI_PM_IBB_PWRUP_PWRDN_DELAY_TYPE; 

typedef enum
{
    EFI_PM_IBB_MODE__IBB_OFF = 0x0,
    EFI_PM_IBB_MODE__WAIT_EOR,          /* = 0x1 */
    EFI_PM_IBB_MODE__IBB_PWRUP1 = 0x4,
    EFI_PM_IBB_MODE__IBB_PWRUP2,
    EFI_PM_IBB_MODE__IBB_PWRUP3,
    EFI_PM_IBB_MODE__IBB_PWRUP4,
    EFI_PM_IBB_MODE__IBB_PWRDN1,        /* = 0x8 */
    EFI_PM_IBB_MODE__IBB_PWRDN2,
    EFI_PM_IBB_MODE__IBB_PWRDN3,
    EFI_PM_IBB_MODE__IBB_PWRDN4,
    EFI_PM_IBB_MODE__IBB_PWRDN5,
    EFI_PM_IBB_MODE__IBB_PWRDN6,
    EFI_PM_IBB_MODE__IBB_ON1,           /* = 0xE */
    EFI_PM_IBB_MODE__IBB_ON2,
    EFI_PM_IBB_MODE__INVALID
} EFI_PM_IBB_MODE;

typedef struct
{
   BOOLEAN                           AmoledModeEn;                         /**< FALSE is LCD and TRUE is amoled          */
   BOOLEAN                           IbbModuleRdy;                         /**< ready to be enabled                      */
   BOOLEAN                           IbbModuleEn;                          /**< module enabled or disabled               */
   BOOLEAN                           SwireRdy;                             /**< swire_rdy                                */
   UINT32                            ChgrResistorKohms;                    /**< charger resistor                         */
   UINT32                            Swire_pulse_mv;                       /**< swire output voltage pulse in milli volt */
   BOOLEAN                           OverrideOutputVolt;                   /**< override output voltage                  */
   UINT32                            OutputVolt;                           /**< output voltage set                       */
   EFI_PM_IBB_PWRUP_PWRDN_DELAY_TYPE IbbDelayConfig;                         /**< IBB delay config                         */
   BOOLEAN                           IbbVregOk;                            /**< IBB VREG OK                              */ 
   BOOLEAN                           ShortCircuit;                         /**< IBB Module short circuit detect          */
   BOOLEAN                           IlimitError;                          /**< IBB current limit error                  */
    /* For IBB 3.0/+ */
    BOOLEAN                           Overload;
    EFI_PM_IBB_MODE                   IbbModeStatus;
    UINT32                            SwireEdgeCount;
}EFI_PM_IBB_STATUS_TYPE;

/* To be used for IBB 3.0/+ Revision */
typedef enum
{
    EFI_PM_IBB_PD_CTRL_SWIRE_PD_UPDT__DISABLE,
    EFI_PM_IBB_PD_CTRL_SWIRE_PD_UPDT__ENABLE,
    EFI_PM_IBB_PD_CTRL_SWIRE_PD_UPDT__INVALID
} EFI_PM_IBB_PD_CTRL_SWIRE_PD_UPDT;

typedef enum
{
    EFI_PM_IBB_PD_CTRL_PD_STRENGHT__FULL,
    EFI_PM_IBB_PD_CTRL_PD_STRENGHT__HALF,
    EFI_PM_IBB_PD_CTRL_PD_STRENGHT__INVALID
} EFI_PM_IBB_PD_CTRL_PD_STRENGHT;

typedef struct
{
    BOOLEAN  enable_pd;
    EFI_PM_IBB_PD_CTRL_SWIRE_PD_UPDT  swire_pd_updt;
    EFI_PM_IBB_PD_CTRL_PD_STRENGHT  pd_strenght;
} EFI_PM_IBB_PD_CTRL_TYPE;

typedef enum
{
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE__1b8_LOW,  /* 1/8 low glitch allowed */
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE__3b8_LOW,  /* 3/8 low glitch allowed */
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE__4b8_LOW,  /* 4/8 low glitch allowed */
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE__5b8_LOW,  /* 5/8 low glitch allowed */
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE__INVALID
} EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE;

typedef enum
{
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE__50mA,
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE__100mA,
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE__150mA,
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE__200mA,
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE__INVALID
} EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE;

typedef enum
{
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES__8,
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES__16,
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES__32,
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES__64,
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES__INVALID
} EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES;    /* no. of cycles per step */

typedef struct
{
    BOOLEAN  en_iplimit_stepper;
    EFI_PM_IBB_ILIMIT_DEB_PERCENTAGE  deb_percentage;
    UINT32  startup_iplimit;
    EFI_PM_IBB_ILIMIT_CURRENT_STEP_SIZE  current_step_size;
    EFI_PM_IBB_IPLIMIT_STEPPER_CYCLES  iplimit_stepper_cycles;
} EFI_PM_IBB_IPLIMIT_STARTUP_CTRL_TYPE;

typedef enum
{
    EFI_PM_IBB_IPLIMIT_COUNT__8,
    EFI_PM_IBB_IPLIMIT_COUNT__16,
    EFI_PM_IBB_IPLIMIT_COUNT__32,
    EFI_PM_IBB_IPLIMIT_COUNT__64,
    EFI_PM_IBB_IPLIMIT_COUNT__INVALID
} EFI_PM_IBB_IPLIMIT_COUNT;

typedef struct
{
    BOOLEAN  en_current_limit;
    EFI_PM_IBB_IPLIMIT_COUNT  ilimit_count;
    UINT32  current_max;
} EFI_PM_IBB_CURRENT_LIMIT_CFG_TYPE;

typedef enum
{
    EFI_PM_IBB_PS_THRESHOLD__0uA,
    EFI_PM_IBB_PS_THRESHOLD__0P5uA,
    EFI_PM_IBB_PS_THRESHOLD__1uA,
    EFI_PM_IBB_PS_THRESHOLD__1P5uA,
    EFI_PM_IBB_PS_THRESHOLD__2uA,
    EFI_PM_IBB_PS_THRESHOLD__2P5uA,
    EFI_PM_IBB_PS_THRESHOLD__3uA,
    EFI_PM_IBB_PS_THRESHOLD__3P5uA,
    EFI_PM_IBB_PS_THRESHOLD__INVALID
} EFI_PM_IBB_PS_THRESHOLD;

typedef struct
{
    BOOLEAN  en_ps;
    EFI_PM_IBB_PS_THRESHOLD  ps_threshold;
} EFI_PM_IBB_PS_CTRL_TYPE;

typedef enum
{
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB__1mS,
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB__2mS,
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB__4mS,
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB__8mS,
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB__INVALID
} EFI_PM_IBB_VREG_OK_OVERLOAD_DEB;

typedef enum
{
    EFI_PM_IBB_VREG_OK_VOK_DEB__4uS,
    EFI_PM_IBB_VREG_OK_VOK_DEB__8uS,
    EFI_PM_IBB_VREG_OK_VOK_DEB__16uS,
    EFI_PM_IBB_VREG_OK_VOK_DEB__32uS,
    EFI_PM_IBB_VREG_OK_VOK_DEB__INVALID
} EFI_PM_IBB_VREG_OK_VOK_DEB;

typedef struct
{
    BOOLEAN  en_overload_blank;
    EFI_PM_IBB_VREG_OK_OVERLOAD_DEB  overload_deb;
    EFI_PM_IBB_VREG_OK_VOK_DEB  vok_deb;
} EFI_PM_IBB_VREG_OK_CTRL_TYPE;

typedef enum
{
    EFI_PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__SLOW,
    EFI_PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__FAST,
    EFI_PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED__INVALID
} EFI_PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED;

typedef enum
{
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p1mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p2mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__0p5mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__1mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__2mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__10mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__12mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__15mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST__INVALID
} EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST;

typedef enum
{
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p1mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p2mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__0p5mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__1mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__2mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__10mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__12mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__15mS,
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW__INVALID
} EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW;

typedef struct
{
    BOOLEAN  en_digital_slew_ctl;
    EFI_PM_IBB_SLEW_CTRL_SLEW_RATE_SPEED  slew_rate_speed;
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_FAST  trans_time_fast;
    EFI_PM_IBB_SLEW_CTRL_TRANS_TIME_SLOW  trans_time_slow;
} EFI_PM_IBB_SLEW_CTRL_TYPE;


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*EFI_PM_IBB_LCD_AMOLED_SEL*/
/** @ingroup
  @par Summary 
  This function configures module for AMOLED or LCD
  
  @param[in] PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in] EnAmoledMode      TRUE enables for AMOLED and FALSE for LCD 
  
  @return
  EFI_SUCCESS            -- Function completed successfully. 
  EFI_DEVICE_ERROR       -- Physical device reported an error.n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_LCD_AMOLED_SEL)(
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN EnAmoledMode
    );


/*EFI_PM_IBB_MODULE_RDY*/
/** @ingroup
  @par Summary 
  This API prepares module to be enabled or not. When this is set 
  to low, the IBB module cannot be enabled (powered-up) through 
  S-Wire or IBB_ENABLE register 
  
  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] IbbRdy             TRUE if ready to be enabled else FALSE
 
  @return
  EFI_SUCCESS            -- Function completed successfully. 
  EFI_DEVICE_ERROR       -- Physical device reported an error. 
*/
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_MODULE_RDY)(
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN IbbRdy
    );


/*EFI_PM_IBB_CONFIG_IBB_CTRL*/
/** @ingroup
  @par Summary 
   This API enables/disables module and makes S-wire ready for listening
  
  @param[in] PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in] EnModule      TRUE enables module and false disables module
  @param[in] SwireRdy      TRUE ready to liste to to S-wire and FALSE not ready to listen to S-Wire
   
  @return
  EFI_SUCCESS            -- Function completed successfully. 
  EFI_DEVICE_ERROR       -- Physical device reported an error. 
 */
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_CONFIG_IBB_CTRL)(
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN EnModule,
    IN BOOLEAN SwireRdy
    );


/*EFI_PM_IBB_SET_SOFT_STRT_CHGR_RESISTOR*/
/** @ingroup
  @par Summary 
  This API sets the charging resistance for soft start. 
  Time constant is ref_cap*charging resistor.
  
  @param[in] PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in] KOhms     Charging resistor value. Values are 16 to 300 KOhms
                                 
 
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
 */
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_SET_SOFT_STRT_CHGR_RESISTOR)(
    IN UINT32  PmicDeviceIndex,
    IN UINT32  KOhms
    );


/*EFI_PM_IBB_SET_SWIRE_OUTPUT_PULSE*/
/** @ingroup
  @par Summary 
  This API sets startup voltage when there is only one s-wire 
  voltage programming pulse
  
  @param[in] PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in] VoltInMilliVolt   Valid value range is 1400 to 7700 milli volt
                                 
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_SET_SWIRE_OUTPUT_PULSE)(
    IN UINT32  PmicDeviceIndex,
    IN UINT32  VoltInMilliVolt
    );


/*EFI_PM_IBB_CONFIG_OUTPUT_VOLT*/
/** @ingroup
  @par Summary 
  This API allows to select default voltage and set output 
  voltage
  
  @param[in] PmicDeviceIndex.     Primary: 0 Secondary: 1
  @param[in] OverrideOutputVolt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
  @param[in] OutputMillivolt      Output voltage. Range is 1400 to 7700 milli volt.
 
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_CONFIG_OUTPUT_VOLT)(
    IN UINT32   PmicDeviceIndex,
    IN BOOLEAN  OverrideOutputVolt,
    IN UINT32   OutputMillivolt
    );


/*EFI_PM_IBB_GET_STATUS*/
/** @ingroup
  @par Summary 
   This API reads the IBB module setting status
  
  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] IbbStatus          Refer struct #EFI_PM_IBB_STATUS_TYPE for more info
        
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
 */
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_GET_STATUS)(
    IN  UINT32                   PmicDeviceIndex,
    OUT EFI_PM_IBB_STATUS_TYPE   *IbbStatus
    );


/*EFI_PM_IBB_CONFIG_PWRUP_PWRDN_DLY*/
/** @ingroup
  @par Summary 
   This configures the delay for IBB module 
  
  @param[in] PmicDeviceIndex.     Primary: 0 Secondary: 1
  @param[in] DelayConfig          Refer struct #EFI_PM_IBB_PWRUP_PWRDN_DELAY_TYPE for more info
        
  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
 */
typedef
EFI_STATUS (EFIAPI * EFI_PM_IBB_CONFIG_PWRUP_PWRDN_DLY)(
    IN  UINT32                              PmicDeviceIndex,
    OUT EFI_PM_IBB_PWRUP_PWRDN_DELAY_TYPE   *DelayConfig
    );


/* For IBB 3.0/+ Revision */
/* EFI_PM_IBB_SET_DEFAULT_VOLTAGE */
/** @ingroup
  @par Summary
  This API configures IBB Output Voltage.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] OpVoltageMv        Voltage level: 400mV to 6300mV

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_SET_DEFAULT_VOLTAGE) (
    IN UINT32  PmicDeviceIndex,
    IN UINT32  OpVoltageMv
    );

/* EFI_PM_IBB_SET_SWIRE_RDY */
/** @ingroup
  @par Summary
  This API configures IBB to ready to listen to S-Wire.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] SwireRdyEn         TRUE if ready to be enabled else FALSE

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_SET_SWIRE_RDY) (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN  SwireRdyEn
    );

/* EFI_PM_IBB_PD_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB PullDown Controls.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] PdCtrl             Refer struct #EFI_PM_IBB_PD_CTRL_TYPE for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_PD_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_IBB_PD_CTRL_TYPE  *PdCtrl
    );

/* EFI_PM_IBB_IPLIMIT_STARTUP_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB Current StartUp Controls.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] IplimitCtrl        Refer struct #EFI_PM_IBB_IPLIMIT_STARTUP_CTRL_TYPE for more info
                                ->startup_iplimit = 175mA to 925mA

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_IPLIMIT_STARTUP_CTRL) (
    IN UINT32 PmicDeviceIndex,
    IN EFI_PM_IBB_IPLIMIT_STARTUP_CTRL_TYPE  *IplimitCtrl
    );

/* EFI_PM_IBB_CURRENT_LIMIT_CFG */
/** @ingroup
  @par Summary
  This API configures IBB Output Current Limit.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] CurrentLimitCfg    Refer struct #EFI_PM_IBB_CURRENT_LIMIT_CFG_TYPE for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_CURRENT_LIMIT_CFG) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_IBB_CURRENT_LIMIT_CFG_TYPE  *CurrentLimitCfg
    );

/* EFI_PM_IBB_PS_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB Pulse Skip Controls.

  @param[in]  PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in]  EnPs              TRUE/FALSE
  @param[in]  PsThreshold       Refer struct #EFI_PM_IBB_PS_THRESHOLD for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_PS_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN  EnPs,
    IN EFI_PM_IBB_PS_THRESHOLD  PsThreshold
    );

/* EFI_PM_IBB_VREG_OK_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB VREG_OK Controls.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] VregOkCtrl         Refer struct #EFI_PM_IBB_VREG_OK_CTRL_TYPE for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_VREG_OK_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_IBB_VREG_OK_CTRL_TYPE  *VregOkCtrl
    );

/* EFI_PM_IBB_SWIRE_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB S-Wire Settings/Controls.

  @param[in] PmicDeviceIndex.   Primary: 0 Secondary: 1
  @param[in] EnSwireVoutUpdt    TRUE/FALSE
  @param[in] OpVoltageAtOneEdge Voltage level: 400mV to 6300mV

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_SWIRE_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN EnSwireVoutUpdt,
    IN UINT32  OpVoltageAtOneEdge
    );

/* EFI_PM_IBB_SLEW_CTRL */
/** @ingroup
  @par Summary
  This API configures IBB Output Slew Rate Controls.

  @param[in] PmicDeviceIndex.  Primary: 0 Secondary: 1
  @param[in] SlewCtrl          Refer struct #EFI_PM_IBB_SLEW_CTRL_TYPE for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_IBB_SLEW_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_IBB_SLEW_CTRL_TYPE  *SlewCtrl
    );


/*===========================================================================

                        PMIC IBB PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup @ingroup efi_pmicIbb_protocol
@par Summary
Qualcomm PMIC IBB Protocol interface.

@par Parameters
@inputprotoparams{pmic_ibb_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_IBB_PROTOCOL {
  UINT64                                  Revision;
  EFI_PM_IBB_LCD_AMOLED_SEL               LcdAmoledSel;
  EFI_PM_IBB_MODULE_RDY                   ModuleRdy;
  EFI_PM_IBB_CONFIG_IBB_CTRL              ConfigIbbCtrl;
  EFI_PM_IBB_SET_SOFT_STRT_CHGR_RESISTOR  ChgrResistor;
  EFI_PM_IBB_SET_SWIRE_OUTPUT_PULSE       SetSwireOutPulse;
  EFI_PM_IBB_CONFIG_OUTPUT_VOLT           ConfigOutVolt;
  EFI_PM_IBB_GET_STATUS                   GetStatus;
  EFI_PM_IBB_CONFIG_PWRUP_PWRDN_DLY       ConfigPwrupPwrdnDly;
    /* For IBB 3.0/+ Revision */
    EFI_PM_IBB_SET_DEFAULT_VOLTAGE          IbbSetDefaultVolgate;
    EFI_PM_IBB_PD_CTRL                      IbbPdCtrl;
    EFI_PM_IBB_IPLIMIT_STARTUP_CTRL         IbbIplimitStartupCtrl;
    EFI_PM_IBB_CURRENT_LIMIT_CFG            IbbCurrentLimitCfg;
    EFI_PM_IBB_PS_CTRL                      IbbPsCtrl;
    EFI_PM_IBB_VREG_OK_CTRL                 IbbVregOkCtrl;
    EFI_PM_IBB_SWIRE_CTRL                   IbbSwireCtrl;
    EFI_PM_IBB_SLEW_CTRL                    IbbSlewCtrl;
};


#endif  /* __EFIPMICIBB_H__ */
