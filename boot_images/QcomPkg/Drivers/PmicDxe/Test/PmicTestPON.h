#ifndef __PMICTESTPON_H__
#define __PMICTESTPON_H__

/*! @file PMICTESTPON.h
 *
 *  PMIC Test PON
 *
 *  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/20/16   sarpit      Added Pmic Test PON Header file

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Protocol/EFIPmicPon.h>
#include <Protocol/EFIPmicPwrOn.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
  EFI_PM_PON_RESET_CFG_WARM_RESET,
  EFI_PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  /* Shutdown to a state of main battery removal */
  EFI_PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
   /* Shutdown to a state of main battery and coin cell removal*/
  EFI_PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
   /* Shutdown + Auto pwr up */
  EFI_PM_PON_RESET_CFG_HARD_RESET,
  /* Main Batt and coin cell remove Shutdown + Auto pwr up 8: DVDD_HARD_RESET */
  EFI_PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  EFI_PM_PON_RESET_CFG_D_VDD_COIN_CELL_REMOVE_HARD_RESET,
  EFI_PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  EFI_PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  EFI_PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN,
  EFI_PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  EFI_PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  EFI_PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  EFI_PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  EFI_PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  EFI_PM_PON_RESET_CFG_INVALID  
}EFI_PM_PON_RESET_CFG_TYPE;

typedef enum
{
  EFI_PM_PON_RESET_SOURCE_KPDPWR,           /* Power key */
  EFI_PM_PON_RESET_SOURCE_RESIN,            /* Resin in form MSM */
  EFI_PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, /* Pwr Key + Resin */
  EFI_PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR,  /* Pwr Key or Resin */
  EFI_PM_PON_RESET_SOURCE_GP1,              /* General purpose-1 */
  EFI_PM_PON_RESET_SOURCE_GP2,              /* General purpose-1 */
  EFI_PM_PON_RESET_SOURCE_INVALID  
}EFI_PM_PON_RESET_SOURCE_TYPE;

typedef enum
{
   EFI_PM_PON_OFF,
   EFI_PM_PON_ON,
   EFI_PM_PON_INVALID
}EFI_PM_PON_ON_OFF_TYPE;

typedef enum
{
  EFI_PM_PON_FSM_STATUS_OFF,           /* off state */
  EFI_PM_PON_FSM_STATUS_PON,           /* pon state */
  EFI_PM_PON_FSM_STATUS_POFF,          /* poff state */
  EFI_PM_PON_FSM_STATUS_ON,            /* on state */
  EFI_PM_PON_FSM_STATUS_DVDD_CFG,      /* dvdd_cfg state */
  EFI_PM_PON_FSM_STATUS_RESERVED,      /* reserved */
  EFI_PM_PON_FSM_STATUS_FAULT,         /* fault state  */
  EFI_PM_PON_FSM_STATUS_WARM_RESET,    /* warm reset state */
  EFI_PM_PON_FSM_STATUS_INVALID  
}EFI_PM_PON_FSM_STATUS_TYPE;

typedef struct
{
  uint32 soft : 1;               /* Software trigger */
  uint32 ps_hold : 1;            /* PS_HOLD trigger */             
  uint32 pmic_wd : 1;            /* PMIC Watchdog trigger */             
  uint32 gp1 : 1;                /* Keypad_Reset1 trigger */             
  uint32 gp2 : 1;                /* Keypad_Reset2 trigger */             
  uint32 kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  uint32 resin : 1;              /* RESIN_N trigger */
  uint32 kpdpwr : 1;             /* KPDPWR_N trigger */
  uint32 reserved : 3;           /* Reserved bits */
  uint32 charger : 1;            /* Charger (BOOT_DONE) trigger */
  uint32 tft : 1;                /* TFT trigger */
  uint32 uvlo : 1;               /* UVLO trigger */
  uint32 otst3 : 1;              /* Overtemp trigger */
  uint32 stage3 : 1;             /* Stage3 reset trigger */
}EFI_PM_PON_POFF_REASON_TYPE;

typedef struct
{
  uint32 soft : 1;               /* Software trigger */
  uint32 ps_hold : 1;            /* PS_HOLD trigger */             
  uint32 pmic_wd : 1;            /* PMIC Watchdog trigger */             
  uint32 gp1 : 1;                /* Keypad_Reset1 trigger */             
  uint32 gp2 : 1;                /* Keypad_Reset2 trigger */             
  uint32 kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  uint32 resin : 1;              /* RESIN_N trigger */
  uint32 kpdpwr : 1;             /* KPDPWR_N trigger */
  uint32 reserved : 4;           /* Reserved bits */
  uint32 tft : 1;                /* TFT trigger */
  uint32 reserved1 : 3;          /* Reserved bits */
}PM_PON_WARM_RESET_REASON_TYPE;


typedef struct
{
  uint32 soft : 1;               /* Software trigger */
  uint32 ps_hold : 1;            /* PS_HOLD trigger */             
  uint32 pmic_wd : 1;            /* PMIC Watchdog trigger */             
  uint32 gp1 : 1;                /* Keypad_Reset1 trigger */             
  uint32 gp2 : 1;                /* Keypad_Reset2 trigger */             
  uint32 kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  uint32 resin : 1;              /* RESIN_N trigger */
  uint32 kpdpwr : 1;             /* KPDPWR_N trigger */
  uint32 reserved : 4;           /* Reserved bits */
  uint32 tft : 1;                /* TFT trigger */
  uint32 reserved1 : 3;          /* Reserved bits */
}PM_PON_SOFT_RESET_REASON_TYPE;

typedef struct
{
  uint32 reserved1 : 6;     /* Reserved bits */
  uint32 warm_seq  : 1;     /* Warm sequence */
  uint32 pon_seq   : 1;     /* PON sequence  */
}PM_PON_ON_STATE_REASON_TYPE;


typedef struct
{
  uint32 hard_reset : 1;     /* Hard reset event trigger */
  uint32 smpl : 1;           /* SMPL trigger */             
  uint32 rtc : 1;            /* RTC trigger */             
  uint32 dc_chg : 1;         /* DC Charger trigger */             
  uint32 usb_chg : 1;        /* USB Charger trigger */             
  uint32 pon1 : 1;           /* PON1 trigger */
  uint32 cblpwr : 1;         /* CBL_PWR1_N trigger */
  uint32 kpdpwr : 1;         /* KPDPWR_N trigger */
}PM_PON_REASON_TYPE;

typedef struct
{
  uint32 reserved1 : 4;             /* Reserved bits */  
  uint32 fault_n : 1;              /* Triggered by FAULT_N being low more than 10 clock cycles (32k clock) */
  uint32 pbs_watchdog_to : 1;      /* Triggered by PBS_WATCHDOG_TO in FAULT state */
  uint32 pbs_nack : 1;             /* Triggered by PBS_NACK in FAULT state */
  uint32 kpdpwr_andor_resin : 1;   /* KPDPWR_N, or RESIN_N, or KPDPWR_AND_RESIN, or KPDPWR_OR_RESIN   */
}EFI_PM_PON_STAGE3_RESET_REASON_TYPE;

typedef struct
{
  UINT32 gp_fault0 : 1;         /* Triggered by general-purpose fault #0 */
  UINT32 gp_fault1 : 1;         /* Triggered by general-purpose fault #1 */
  UINT32 gp_fault2 : 1;         /* general-purpose fault #2 */
  UINT32 gp_fault3 : 1;         /* general-purpose fault #3 */  
  UINT32 mbg_fault : 1;         /* MBG fault event */
  UINT32 ovlo : 1;              /* OVLO event    */
  UINT32 uvlo : 1;              /* UVLO event    */
  UINT32 avdd_rb : 1;           /* AVDD_RB event */          
  UINT32 reserved1 : 3;         /* Reserved bits */  
  UINT32 fault_n : 1;           /* FAULT_N bus */
  UINT32 pbs_watchdog_to : 1;   /* PBS WATCHDOG */
  UINT32 pbs_nack : 1;          /* PBS NACK event */    
  UINT32 restart_pon : 1;       /* RESTART PON */  
  UINT32 otst3 : 1;             /* OTST3 */
}EFI_PM_PON_FAULT_STATE_REASON_TYPE;

typedef struct
{
  UINT32 reserved1 : 2;               /* Reserved bits */
  UINT32 raw_xvdd_rb_occured : 1;     /* raw_xvdd_rb event occured */
  UINT32 raw_dvdd_rb_occured : 1;     /* raw_dvdd_rb event occured  */
  UINT32 immediate_xvdd_shutdown : 1; /* immediate_xvdd_shutdown s2 Reset */
  UINT32 s3_reset : 1;                /* Stage 3 Reset */
  UINT32 fault_seq : 1;               /* fault-handling sequence */
  UINT32 poff_seq : 1;                /* normal powering-off sequence */
}EFI_PM_PON_OFF_STATE_REASON_TYPE;

typedef struct _EFI_PM_TEST_PON_PS_HOLD_CFG_PARAM
{
    UINT8       DeviceIndex;
    EFI_PM_PON_RESET_CFG_TYPE ResetCfgType;
}EFI_PM_TEST_PON_PS_HOLD_CFG_PARAM, EFI_PM_TEST_PON_OVERTEMP_RESET_CFG_PARAM;

typedef struct _EFI_PM_TEST_PON_RESET_SOURCE_CFG_PARAM
{
    UINT8       DeviceIndex;
    EFI_PM_PON_RESET_SOURCE_TYPE ResetSourceType;
    UINT32 S1_timer;
    UINT32 S2_timer;
    EFI_PM_PON_RESET_CFG_TYPE ResetCfgType;
}EFI_PM_TEST_PON_RESET_SOURCE_CFG_PARAM;

typedef struct _EFI_PM_TEST_PON_RESET_SOURCE_CTL_PARAM
{
    UINT8       DeviceIndex;
    EFI_PM_PON_RESET_SOURCE_TYPE ResetSourceType;
    EFI_PM_PON_ON_OFF_TYPE On_Off;
}EFI_PM_TEST_PON_RESET_SOURCE_CTL_PARAM;

typedef struct _EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG_PARAM
{
    UINT8       DeviceIndex;
    EFI_PM_PON_RESET_SOURCE_TYPE ResetSourceType;
    UINT32 S3_timer;
}EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_POFF_REASON_PARAM
{
    UINT8       DeviceIndex;
    EFI_PM_PON_POFF_REASON_TYPE  *Reason;
}EFI_PM_TEST_PON_GET_POFF_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_WARM_RESET_PARAM
{
    UINT8       DeviceIndex;
    PM_PON_WARM_RESET_REASON_TYPE  *Reason;
}EFI_PM_TEST_PON_GET_WARM_RESET_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_SOFT_RESET_REASON_PARAM
{
    UINT8       DeviceIndex;
    PM_PON_SOFT_RESET_REASON_TYPE  *Reason;
}EFI_PM_TEST_PON_GET_SOFT_RESET_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_ALL_REASONS_PARAM
{
    UINT8    DeviceIndex;
    UINT64   *Reason;
}EFI_PM_TEST_PON_GET_ALL_REASONS_PARAM;

typedef struct _EFI_PM_TEST_PON_DEBOUNCE_CTL_PARAM
{
    UINT8    DeviceIndex;
    UINT32   Time_delay;
}EFI_PM_TEST_PON_DEBOUNCE_CTL_PARAM;

typedef struct _EFI_PM_TEST_PON_CONFIG_UVLO_PARAM
{
    UINT8    DeviceIndex;
    UINT32   Uvlo_rising_thresh_voltage_in_mv;
    UINT32   Hyst_voltage_in_mv;
	BOOLEAN  uvlo_enable;
}EFI_PM_TEST_PON_CONFIG_UVLO_PARAM;

typedef struct _EFI_PM_TEST_PON_WARM_RESET_STATUS_PARAM
{
    UINT8    DeviceIndex;
    boolean * Status;
}EFI_PM_TEST_PON_WARM_RESET_STATUS_PARAM,EFI_PM_TEST_PON_UVLO_RESET_STATUS_PARAM;

typedef struct _EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR_PARAM
{
    UINT8    DeviceIndex;
}EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR_PARAM;

typedef struct _EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS_PARAM
{
    UINT8    DeviceIndex;
    EFI_PM_PON_TRIGGER_TYPE TriggerType;
    boolean * Status;
}EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE_PARAM
{
    UINT8    DeviceIndex;
    EFI_PM_PON_FSM_STATUS_TYPE *pon_fsm_status;
}EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_ON_REASON_PARAM
{
    UINT8    DeviceIndex;
    PM_PON_ON_STATE_REASON_TYPE *Reason;
}EFI_PM_TEST_PON_GET_ON_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_PON_REASON_PARAM
{
    UINT8    DeviceIndex;
    PM_PON_REASON_TYPE *Reason;
}EFI_PM_TEST_PON_GET_PON_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON_PARAM
{
    UINT8    DeviceIndex;
    EFI_PM_PON_STAGE3_RESET_REASON_TYPE *Reason;
}EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_FAULT_REASON_PARAM
{
    UINT8    DeviceIndex;
    EFI_PM_PON_FAULT_STATE_REASON_TYPE *Reason;
}EFI_PM_TEST_PON_GET_FAULT_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_GET_OFF_REASON_PARAM
{
    UINT8    DeviceIndex;
    EFI_PM_PON_OFF_STATE_REASON_TYPE *Reason;
}EFI_PM_TEST_PON_GET_OFF_REASON_PARAM;

typedef struct _EFI_PM_TEST_PON_WDOG_CFG_PARAM
{
    UINT8       DeviceIndex;
    UINT32 S1_timer;
    UINT32 S2_timer;
    EFI_PM_PON_RESET_CFG_TYPE ResetCfgType;
}EFI_PM_TEST_PON_WDOG_CFG_PARAM;

typedef struct _EFI_PM_TEST_PON_WDOG_ENABLE_PARAM
{
	UINT8       DeviceIndex;
    EFI_PM_PON_ON_OFF_TYPE enable;
}EFI_PM_TEST_PON_WDOG_ENABLE_PARAM;

typedef struct _EFI_PM_TEST_PON_WDOG_PET_PARAM
{
	UINT8       DeviceIndex;
}EFI_PM_TEST_PON_WDOG_PET_PARAM;

typedef struct _EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK_PARAM
{
	BOOLEAN    * is_key_hold_valid;
}EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK_PARAM;

/*
typedef struct _EFI_PM_TEST_PON_WDOG_STATUS_GET_PARAM
{
	UINT8       DeviceIndex;
	BOOLEAN    *dog_status;
}EFI_PM_TEST_PON_WDOG_STATUS_GET_PARAM; */

typedef enum
{
    EFI_PM_TEST_PON_PS_HOLD_CFG,
    EFI_PM_TEST_PON_RESET_SOURCE_CFG,
    EFI_PM_TEST_PON_OVERTEMP_RESET_CFG,
    EFI_PM_TEST_PON_RESET_SOURCE_CTL,
    EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG,
    EFI_PM_TEST_PON_GET_POFF_REASON,
    EFI_PM_TEST_PON_GET_WARM_RESET,
    EFI_PM_TEST_PON_GET_SOFT_RESET_REASON,
    EFI_PM_TEST_PON_GET_ALL_REASONS,
    EFI_PM_TEST_PON_DEBOUNCE_CTL,
    EFI_PM_TEST_PON_CONFIG_UVLO,
    EFI_PM_TEST_PON_WARM_RESET_STATUS,
    EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR,
    EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS,
   // EFI_PM_TEST_PON_GET_SPARE_REG_DATA,
    EFI_PM_TEST_PON_UVLO_RESET_STATUS,
	EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE,
	EFI_PM_TEST_PON_GET_ON_REASON,
	EFI_PM_TEST_PON_GET_PON_REASON,
	EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON,
	EFI_PM_TEST_PON_GET_FAULT_REASON,
	EFI_PM_TEST_PON_GET_OFF_REASON,
	EFI_PM_TEST_PON_WDOG_CFG,
	EFI_PM_TEST_PON_WDOG_ENABLE,
	EFI_PM_TEST_PON_WDOG_PET,
	EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK,
//	EFI_PM_TEST_PON_WDOG_STATUS_GET,
}EFI_PM_TEST_DRIVER_PON_TYPE_FUNCTION;

typedef union _EFI_PM_TEST_PON_FUNCTION_TYPE
{
    EFI_PM_TEST_PON_PS_HOLD_CFG_PARAM           PonPsHoldCfg;
    EFI_PM_TEST_PON_RESET_SOURCE_CFG_PARAM      PonResetSourceCfg;
    EFI_PM_TEST_PON_OVERTEMP_RESET_CFG_PARAM    PonOvertempResetCfg;
    EFI_PM_TEST_PON_RESET_SOURCE_CTL_PARAM      PonResetSourceCtl;
    EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG_PARAM PonStage3ResetSourceCfg;
    EFI_PM_TEST_PON_GET_POFF_REASON_PARAM       PonGetPoffReason;
    EFI_PM_TEST_PON_GET_WARM_RESET_PARAM        PonGetGetWarmResetReason;
    EFI_PM_TEST_PON_GET_SOFT_RESET_REASON_PARAM       PonGetSoftResetReason;
    EFI_PM_TEST_PON_GET_ALL_REASONS_PARAM    PonGetAllPonReasons;
    EFI_PM_TEST_PON_DEBOUNCE_CTL_PARAM       PondebounceCtl;
    EFI_PM_TEST_PON_CONFIG_UVLO_PARAM        PonConfigUVLO;
    EFI_PM_TEST_PON_WARM_RESET_STATUS_PARAM  PonWarmResetStatus;
    EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR_PARAM  PonWarmResetStatusClear;
    EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS_PARAM  PonTriggerEnableStatus;
   // EFI_PM_TEST_PON_GET_SPARE_REG_DATA_PARAM  PonGetSpareRegData;
    EFI_PM_TEST_PON_UVLO_RESET_STATUS_PARAM   PonUvloResetStatus;
	EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE_PARAM PonGetFsmCurrentState;
	EFI_PM_TEST_PON_GET_ON_REASON_PARAM        PonGetonReason;
	EFI_PM_TEST_PON_GET_PON_REASON_PARAM       PonGetPonReason;
	EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON_PARAM PonStage3ResetReason;
	EFI_PM_TEST_PON_GET_FAULT_REASON_PARAM     PonGetFaultReason;
	EFI_PM_TEST_PON_GET_OFF_REASON_PARAM       PonGetOffReason;
	EFI_PM_TEST_PON_WDOG_CFG_PARAM             PonWdogCfg;
	EFI_PM_TEST_PON_WDOG_ENABLE_PARAM		   PonWdogEnable;
	EFI_PM_TEST_PON_WDOG_PET_PARAM			   PonWdogPet;
	EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK_PARAM   PonPwrKeyHoldCheck;
	// EFI_PM_TEST_PON_WDOG_STATUS_GET_PARAM      PonWdogGetStatus;
	
	
}EFI_PM_TEST_PON_FUNCTION_TYPE;



/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTPON_H__


