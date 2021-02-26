#ifndef __PM_PON_H__
#define __PM_PON_H__
/*! \file pm_pon.h
 *  \n
 *  \brief This file contains functions and variable declarations to support 
 *   the pon driver module.
 *
 *  \n &copy; Copyright (c) 2012-2017 by Qualcomm Technologies Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/16   akm     Added API to update OPTION BIT register
04/29/16   aab     Updated pm_app_pon_reset_cfg_type and pm_pon_reset_cfg_type
03/22/16   aab     Added pm_pon_get_fault_reasons() and removed pm_pon_gen2_config_uvlo()
01/22/16   al      Added API to write to spare registers
10/08/15   aab     Added pm_app_pon_pshold_cfg(), pm_app_pon_reset_cfg() and pm_app_pon_s3_reset_timer_cfg()
10/05/15   aab     Added pm_app_pon_reset_cfg_type and pm_app_pon_reset_source_type. Removed pmapp_ps_hold_cfg_type.
05/31/15   aab     Added pm_log_pon_reasons() API
01/06/15   kt      Added pmapp_ps_hold_cfg API for multi-pmic config
08/19/14   al      Adding API to configure PON trigger
12/04/13   aab     Added PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET to pm_pon_reset_cfg_type
11/01/13   sv      Applying CR538994 Changes with power up failure fix. (CR-570053)
10/24/13   sv      Reverting CR538994 Changes (CR-566323)
06/24/13   ps      Added PON Stage3 reset config API CR#495834 
04/10/13   kt      Added PMIC Watchdog config support  
02/27/13   kt      Added PON IRQ related APIs  
12/06/12   hw      Remove comdef.h and use com_dtypes.h
22/11/12   umr     Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum{
    PM_POWER_RESET_SIGNAL__INVALID
}pm_power_reset_signal;

typedef enum
{
    PM_PWR_ON_RST_ACTION_TYPE__COMPLETE_FULL_SHUTDOWN,
    PM_PWR_ON_RST_ACTION_TYPE__FULL_SHUTDOWN,
    PM_PWR_ON_RST_ACTION_TYPE__HARD_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__POWER_OFF_NORMAL,
    PM_PWR_ON_RST_ACTION_TYPE__SOFT_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__WARM_RESET,
    PM_PWR_ON_RST_ACTION_TYPE__INVALID  
}pm_power_reset_action_type;


/* Power Key hysteresis settings */
typedef enum
{
  PM_PWR_KEY_DELAY_EQ_0_msecs,         /*  0.00  ms */
  PM_PWR_KEY_DELAY_EQ_0p97_msecs,      /*  0.97  ms */
  PM_PWR_KEY_DELAY_EQ_1p95_msecs,      /*  1.95  ms */
  PM_PWR_KEY_DELAY_EQ_3p90_msecs,      /*  3.90  ms */
  PM_PWR_KEY_DELAY_EQ_7p81_msecs,      /*  7.81  ms */
  PM_PWR_KEY_DELAY_EQ_15p62_msecs,     /* 15.62  ms */
  PM_PWR_KEY_DELAY_EQ_31p25_msecs,     /* 31.25  ms */
  PM_PWR_KEY_DELAY_EQ_62p50_msecs,     /* 62.50  ms */
  PM_NUM_OF_PWR_KEY_DELAY              /* Number of power key hysteresis
                                          settings */
}pm_pwr_key_delay_type;  

/* Event definition for PMIC power on status */
#define  PM_PWR_ON_EVENT_KEYPAD     0x1
#define  PM_PWR_ON_EVENT_RTC        0x2
#define  PM_PWR_ON_EVENT_CABLE      0x4
#define  PM_PWR_ON_EVENT_SMPL       0x8
#define  PM_PWR_ON_EVENT_WDOG       0x10
#define  PM_PWR_ON_EVENT_USB_CHG    0x20
#define  PM_PWR_ON_EVENT_WALL_CHG   0x40

/*======================= Cable power On definitions ======================*/

typedef enum 
{
   PM_PON_PON_REASON1,
  /*Reasons that PMIC left the off state*/
  PM_PON_WARM_RESET_REASON1,
  /*Reasons that PMIC entered the Warm Reset state*/
  PM_PON_WARM_RESET_REASON2,
  /*Reasons that PMIC entered the Warm Reset state*/
  PM_PON_POFF_REASON1,
  /*Reasons that the PMIC left the on state and commenced a shutdown sequence.*/
  PM_PON_POFF_REASON2,
  /*Reasons that the PMIC left the on state and commenced a shutdown sequence.*/
  PM_PON_SOFT_RESET_REASON1,
  /*Reasons that the PMIC registers were reset.*/
  PM_PON_SOFT_RESET_REASON2,
  /*Reasons that the PMIC registers were reset.*/
  PM_PON_REASON_INVALID
  /*Invalid Reason*/
} pm_pon_reasons_type;



//Support for PON GEN2 Module ---------------------------------
/*! \enum pm_pon_fsm_status_type
   \brief PON FSM State machine status type.
 */
typedef enum
{
  PM_PON_FSM_STATUS_OFF,           /* off state */
  PM_PON_FSM_STATUS_PON,           /* pon state */
  PM_PON_FSM_STATUS_POFF,          /* poff state */
  PM_PON_FSM_STATUS_ON,            /* on state */
  PM_PON_FSM_STATUS_DVDD_CFG,      /* dvdd_cfg state */
  PM_PON_FSM_STATUS_RESERVED,      /* reserved */
  PM_PON_FSM_STATUS_FAULT,         /* fault state  */
  PM_PON_FSM_STATUS_WARM_RESET,    /* warm reset state */
  PM_PON_FSM_STATUS_INVALID  
}pm_pon_fsm_status_type;




/*! \struct pm_pon_on_state_reason_type
   \brief Power On reason type.
 */
typedef struct
{
  unsigned reserved1 : 6;     /* Reserved bits */
  unsigned warm_seq  : 1;     /* Warm sequence */
  unsigned pon_seq   : 1;     /* PON sequence  */
}pm_pon_on_state_reason_type;


/*! \struct pm_pon_off_state_reason_type
   \brief Power Off reason type.
 */
typedef struct
{
  unsigned reserved1 : 2;               /* Reserved bits */
  unsigned raw_xvdd_rb_occured : 1;     /* raw_xvdd_rb event occured */
  unsigned raw_dvdd_rb_occured : 1;     /* raw_dvdd_rb event occured  */
  unsigned immediate_xvdd_shutdown : 1; /* immediate_xvdd_shutdown s2 Reset */
  unsigned s3_reset : 1;                /* Stage 3 Reset */
  unsigned fault_seq : 1;               /* fault-handling sequence */
  unsigned poff_seq : 1;                /* normal powering-off sequence */
}pm_pon_off_state_reason_type;



/*! \struct pm_pon_fault_state_reason_type
   \brief Pon fault reason type.
 */
typedef struct
{
  unsigned gp_fault0 : 1;         /* Triggered by general-purpose fault #0 */
  unsigned gp_fault1 : 1;         /* Triggered by general-purpose fault #1 */
  unsigned gp_fault2 : 1;         /* general-purpose fault #2 */
  unsigned gp_fault3 : 1;         /* general-purpose fault #3 */  
  unsigned mbg_fault : 1;         /* MBG fault event */
  unsigned ovlo : 1;              /* OVLO event    */
  unsigned uvlo : 1;              /* UVLO event    */
  unsigned avdd_rb : 1;           /* AVDD_RB event */          
  
  unsigned reserved1 : 3;         /* Reserved bits */  
  unsigned fault_n : 1;           /* FAULT_N bus */
  unsigned pbs_watchdog_to : 1;   /* PBS WATCHDOG */
  unsigned pbs_nack : 1;          /* PBS NACK event */    
  unsigned restart_pon : 1;       /* RESTART PON */  
  unsigned otst3 : 1;             /* OTST3 */
}pm_pon_fault_state_reason_type;



/*! \struct pm_pon_stage3_reset_reason_type
   \brief Pon s3 reset reason type.
 */
typedef struct
{
  unsigned reserved1 : 4;             /* Reserved bits */  
  unsigned fault_n : 1;              /* Triggered by FAULT_N being low more than 10 clock cycles (32k clock) */
  unsigned pbs_watchdog_to : 1;      /* Triggered by PBS_WATCHDOG_TO in FAULT state */
  unsigned pbs_nack : 1;             /* Triggered by PBS_NACK in FAULT state */
  unsigned kpdpwr_andor_resin : 1;   /* KPDPWR_N, or RESIN_N, or KPDPWR_AND_RESIN, or KPDPWR_OR_RESIN   */
}pm_pon_stage3_reset_reason_type;



//--------------------------------------------------------------



/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  PM_PON_RESET_CFG_WARM_RESET,
  /* Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_HARD_RESET,
  /*Normal shutdown*/
  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,  
  /* Main Batt and coin cell remove Shutdown + Auto pwr up 8: DVDD_HARD_RESET */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  /* Shutdown to a state of main battery removal */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
   /* Shutdown to a state of main battery and coin cell removal*/
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,  
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_INVALID  
}pm_pon_reset_cfg_type;

/*! \enum pm_pon_reset_source_type
   \brief Reset source type.
   DO NOT CHANGE THE SEQUENCE OF TOP 4 enums
 */
typedef enum
{
  PM_PON_RESET_SOURCE_KPDPWR,           /* Power key */
  PM_PON_RESET_SOURCE_RESIN,            /* Resin in form MSM */
  PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, /* Pwr Key + Resin */
  PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR,  /* Pwr Key or Resin */
  PM_PON_RESET_SOURCE_GP1,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_GP2,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_INVALID  
}pm_pon_reset_source_type;

/*! \struct pm_pon_reason_type
   \brief Power On reason type.
 */
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
}pm_pon_reason_type;


/*! \struct pm_pon_poff_reason_type
   \brief Power Off reason type.
 */
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
}pm_pon_poff_reason_type;

/*! \struct pm_pon_warm_reset_reason_type
   \brief Warm Reset reason type.
 */
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
}pm_pon_warm_reset_reason_type;

/*! \struct pm_pon_soft_reset_reason_type
   \brief Soft Reset reason type.
 */
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
}pm_pon_soft_reset_reason_type;


/*! \enum pm_pon_irq_type
   \brief different types of irq bit fields of pon irq module
 */
typedef enum {
  PM_PON_IRQ_KPDPWR_ON          = 0,  
  PM_PON_IRQ_RESIN_ON           = 1,  
  PM_PON_IRQ_CBLPWR_ON          = 2,  
  PM_PON_IRQ_KPDPWR_BARK        = 3,  
  PM_PON_IRQ_RESIN_BARK         = 4,  
  PM_PON_IRQ_KPDPWR_RESIN_BARK  = 5,  
  PM_PON_IRQ_PMIC_WD_BARK       = 6,  
  PM_PON_IRQ_SOFT_RESET 	      = 7,
  /*INVALID*/
  PM_PON_IRQ_INVALID
}pm_pon_irq_type;


/*! \enum pm_pon_pbl_status
   \brief types of PON_PBL RB occurance 
 */
typedef enum {
  PM_PON_PBL_DVDD_RB_OCCURRED,
  PM_PON_PBL_XVDD_RB_OCCURRED,
  PM_PON_PBL_STATUS_INVALID
} pm_pon_pbl_status_type;


typedef enum {
  PM_PON_PERPH_SPARE,  
  PM_PON_DVDD_SPARE,  
  PM_PON_XVDD_SPARE,  
  PM_PON_SOFT_SPARE,  
  /*INVALID*/
  PM_PON_SPARE_INVALID
}pm_pon_spare_reg_type;

/*! \enum pm_pon_trigger_type
   \brief Power On Triggers
 */
typedef enum 
{
   /*! 1 - Trigger from SMPL.                  */
   PM_PON_TRIGGER_SMPL,
   /*! 2 - Trigger from RTC.                   */
   PM_PON_TRIGGER_RTC,
   /*! 3 - Trigger from DC Charger.            */
   PM_PON_TRIGGER_DC_CHG,
   /*! 4 - Trigger from USB Charger.           */
   PM_PON_TRIGGER_USB_CHG,
   /*! 5 - Trigger from PON1.                  */
   PM_PON_TRIGGER_PON1,
   /*! 6 - Trigger by CBL_PWR   .              */
   PM_PON_TRIGGER_CBLPWR,
   /*! 7 - Trigger by Keypad.                  */
   PM_PON_TRIGGER_KPDPWR,
   PM_PON_TRIGGER_INVALID
}pm_pon_trigger_type;

/*! \enum pm_pon_option_bit_type
   \brief different types of option bit fields of pon option bit register
 */

typedef enum {
  /*! Bit 1 - 5   */	
  PM_PON_OPTION_DC_CHG_REDGE_PON,      /*Rising edge   */
  PM_PON_OPTION_USB_CHG_REDGE_PON,     /*Rising edge   */
  PM_PON_OPTION_CBLPWR_FEDGE_PON,      /*Falling Edge  */
  PM_PON_OPTION_PON1_REDGE_PON,        /*Rising edge   */
  PM_PON_OPTION_KPDPWR_FEDGE_PON,      /*Falling edge  */
  /*INVALID*/
  PM_PON_OPTION_INVALID
}pm_pon_option_bit_type;
 
/*! \enum pmapp_ps_hold_cfg_type
   \brief PS_HOLD reset configure type.
 */
typedef enum
{
  PM_APP_PON_CFG_WARM_RESET,
  PM_APP_PON_CFG_HARD_RESET,
  PM_APP_PON_CFG_NORMAL_SHUTDOWN,
  PM_APP_PON_CFG_DVDD_HARD_RESET,
  PM_APP_PON_CFG_DVDD_SHUTDOWN,
  PM_APP_PON_CFG_MAX
}pm_app_pon_reset_cfg_type;

 
typedef enum
{
  PM_APP_PON_RESET_SOURCE_KPDPWR,              /* Power key */
  PM_APP_PON_RESET_SOURCE_RESIN,               /* Resin in form MSM */
  PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR,    /* Key Combo ... RESIN AND Vol Key use the same pin*/
  PM_APP_PON_RESET_SOURCE_RESIN_OR_KPDPWR,     /* Key Combo ... RESIN OR Vol Key use the same pin*/
  PM_APP_PON_RESET_SOURCE_MAX
}pm_app_pon_reset_source_type;
 
 
  
/*===========================================================================

                        HEADER FILES

===========================================================================*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_ps_hold_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg);

/**
 * @brief Configure PON reset sources
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param reset_source: Select source for reset.
 *            PM_PON_RESET_SOURCE_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN,
 *            PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR,
 *            PM_PON_RESET_SOURCE_GP1,
 *            PM_PON_RESET_SOURCE_GP2,
 *  
 * @param s1_timer: Select S1 timer in ms for bark.
 *            Supported time in ms for all the reset source
 *            types, Rounds UP to next largest setting if
 *            value passed is in between the permitted values:
 *            0, 32, 56, 80, 128, 184, 272, 408, 608, 904, 1352,
 *            2048, 3072, 4480, 6720, 10256
 *  
 * @param s2_timer: Select S2 timer in ms for bite. This is the 
 *                time after bark S1.
 *            Supported time in ms for all the reset source
 *            types, rounds up to next largest number in ms if
 *            value passed is in between the permitted values:0,
 *            10, 50, 100, 250, 500, 1000, 2000
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_pon_reset_source_cfg(uint32 pmic_device_index, 
                        pm_pon_reset_source_type reset_source,
                        uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type);



/**
 * @brief Configure PON Overtemp reset sources
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_overtemp_reset_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type overtemp_cfg);


/**
 * @brief Configure PON reset Control
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param reset_source source of the reset to be 
 *          enabled / disabled.
 *                   
 * @param enable Select enable / disable 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_reset_source_ctl(uint32 pmic_device_index, 
                        pm_pon_reset_source_type reset_source, pm_on_off_type on_off);

/**
 * @brief Configure PON stage3 reset source and timer
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param reset_source: Select source for reset.
 *            PM_PON_RESET_SOURCE_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN,
 *            PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR
 *  
 * @param s3_timer: Select S3 timer in sec
 *            Supported time in sec for all the reset source
 *            types, rounds up to next largest number in sec if
 *            value passed is in between the permitted values:
 *            0, 2, 4, 8, 16, 32, 64, 128
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_stage3_reset_source_cfg(uint32 pmic_device_index, 
                               pm_pon_reset_source_type reset_source,
                               uint32 s3_timer);
                        
/**
 * @brief Configure PON PMIC Watchdog reset source
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param s1_timer: Select S1 timer in seconds for bark.
 *            Supported time in seconds: 0 to 127
 *  
 * @param s2_timer: Select S2 timer in seconds for bite. This is
 *                the time after bark S1.
 *            Supported time in seconds: 0 to 127
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_cfg(uint32 pmic_device_index, uint32 s1_timer, uint32 s2_timer,
                pm_pon_reset_cfg_type reset_cfg_type);

/**
 * @brief PON PMIC Watchdog reset Control (enable/disable)
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *                   
 * @param enable Select enable / disable 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_enable(uint32 pmic_device_index, pm_on_off_type enable);

/**
 * @brief Clears the PMIC Watchdog timer (Pets the PMIC 
 *        Watchdog).
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero. 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_pet(uint32 pmic_device_index);


/**
 * @brief This function returns XVDD/DVDD status 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 *                   pbl_status ,  DVDD_RB_OCCURRED: to get the status of DVDD RB occurance  
                                          XVDD_RB_OCCURRED: to get the status of XVDD RB occurance  
 * @param[out] status:  
 *                XVDD/DVDD status (returns TRUE if warm reset
 *                occured, FALSE otherwise).
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_pbl_get_status(uint32 pmic_device_index, pm_pon_pbl_status_type pbl_status, boolean *status);
/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                PON reason type. Refer enum
 *                pm_pon_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_pon_get_pon_reason(uint32 pmic_device_index, pm_pon_reason_type* reason);


/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                ON reason type. Refer enum
 *                pm_pon_on_state_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type  
pm_pon_get_on_reason(uint32 pmic_device_index, pm_pon_on_state_reason_type* reason);


/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                OFF reason type. Refer enum
 *                pm_pon_off_state_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type 
pm_pon_get_off_reason(uint32 pmic_device_index, pm_pon_off_state_reason_type* reason);


/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                FAULT reason type. Refer enum
 *                pm_pon_fault_state_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type 
pm_pon_get_fault_reason(uint32 pmic_device_index, pm_pon_fault_state_reason_type* reason);



/**
 * @brief Returns reason for Power Off 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                POFF reason type. Refer enum
 *                pm_pon_poff_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_poff_reason(uint32 pmic_device_index, pm_pon_poff_reason_type* reason);

/**
 * @brief Returns reason for Warm Reset
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                Warm reset reason type. Refer enum
 *                pm_pon_warm_reset_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_warm_reset_reason(uint32 pmic_device_index, pm_pon_warm_reset_reason_type* reason);

/**
 * @brief Returns reason for Soft Reset
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                Soft reset reason type. Refer enum
 *                pm_pon_soft_reset_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_soft_reset_reason(uint32 pmic_device_index, pm_pon_soft_reset_reason_type* reason);



/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                S3 Reset reason type. Refer enum
 *                pm_pon_stage3_reset_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type 
pm_pon_get_stage3_reset_reason(uint32 pmic_device_index, pm_pon_stage3_reset_reason_type* reason);




/**
 * @brief Returns reasons for Power On, Power Off, Warm Reset
 *        and Soft Reset.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] pon_reasons:  
 *                Returns a uint64 value with all the PON
 *                reasons (includes PON, POFF, WARM RESET and
 *                SOFT RESET reasons).
 *
 * @param[in] pon_reasons:
 *                PON_GEN1: Returns a uint64 value  of the
 *                   following PON reasons :
 *                   PON_REASON1,PON_REASON2,
 *                   WARM_RESET_REASON1, WARM_RESET_REASON2,
 *                   POFF_Reason1, POFF_Reason2,
 *                   SOFT_Reset_Reason1, SOFT_Reset_Reason2
 *  
 *                PON_GEN2: Returns a uint64 value  of the
 *                   following PON reasons :
 *                   PON_REASON1,,WARM_RESET_REASON1,,ON_REASON,POFF_REASON1,,OFF_REASON
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type 
pm_pon_get_all_pon_reasons(uint32 pmic_device_index, uint64* pon_reasons);



/**
 * @brief Returns falult PON reasons
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param[in] fault_reasons: 
 *                Returns a uint64 value of the following PON reasons:
 *                   FAULT_REASON1
 *                   FAULT_REASON2
 *                   S3_RESET_REASON
 *                   SOFT_RESET_REASON1
 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type 
pm_pon_get_fault_reasons(uint32 pmic_device_index, uint64* fault_reasons);

/**
 * @brief Configure PON Debounce time delay for KPD, CBL, 
 *        General Purpose PON, RESIN, RESIN AND KPD, GP1 and GP2
 *        state change and interrupt triggering.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] time_delay:  
 *                Select time delay in milli seconds. Supported
 *                time in ms, rounds up to the next largest
 *                number in ms if value passed is in between the
 *                permitted values: 15, 31, 62, 125, 250, 500,
 *                1000, 2000
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_pon_debounce_ctl(uint32 pmic_device_index, uint32 time_delay);

/**
 * @brief This function enables or disables pon irq
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding PON interrupt 
 *                FALSE:Disable corresponding PON interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_irq_enable(uint32 pmic_device_index, pm_pon_irq_type irq, boolean enable);


/**
 * @brief This function clears the PON irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_irq_clear(uint32  pmic_device_index, pm_pon_irq_type irq);


/**
 * @brief This function configures the PON irq trigger 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_resources_and_types.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_irq_set_trigger(uint32 pmic_device_index, pm_pon_irq_type irq, pm_irq_trigger_type trigger);


/**
 * @brief This function configures the PON for irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_resources_and_types.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_irq_status(uint32 pmic_device_index, pm_pon_irq_type irq, pm_irq_status_type type, boolean *status);


/**
 * @brief This function configures UVLO Rising Voltage threshold and HYST Voltage threshold
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0
 * @param[in] :  
 *          uvlo_rising_thresh_voltage_in_mv (Range between 2875 and 3225 )
 *          hyst_voltage_in_mv (Range between 175 and 425 )
 *            
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_config_uvlo(uint32 pmic_device_index, uint32 uvlo_thresh_voltage_in_mv, uint32 uvlo_hyst_voltage_in_mv, boolean uvlo_enable);


/**
 * @brief This function returns WARM RESET status whether the 
 *        reset occured or not and this status needs to be
 *        explicitly cleared using the
 *        pm_pon_warm_reset_status_clear API.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[out] status:  
 *                WARM RESET status (returns TRUE if warm reset
 *                occured, FALSE otherwise).
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER = Null pointer passed
 *          in as an argument.
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_warm_reset_status(uint32 pmic_device_index, boolean *status);


/**
 * @brief This function clears the WARM RESET status.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_warm_reset_status_clear(uint32 pmic_device_index);





/**
 * @brief This function is to get the spare register value
 *        pm_pon_get_spare_reg  API.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
                     spare_reg.  PM_PON_PERPH_RB_SPARE,  PM_PON_DVDD_RB_SPARE, 
                                 PM_PON_XVDD_RB_SPARE,  PM_PON_SOFT_RB_SPARE  
 * @param[out] status:  
 *                 returns the value of the specified spare register.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_get_spare_reg (uint32 pmic_device_index, pm_pon_spare_reg_type spare_reg ,uint8* data_ptr);

/**
 * @brief This function is to set the spare register value
 *        pm_pon_get_spare_reg  API.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
                     spare_reg.  PM_PON_PERPH_RB_SPARE,  PM_PON_DVDD_RB_SPARE, 
                                 PM_PON_XVDD_RB_SPARE,  PM_PON_SOFT_RB_SPARE  
 * @param[in] set_value:  
 *                 value to be set of the specified spare register
 *                 bit.
 * @param[in] mask:  
 *                 Bit of spare register to write
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 */
pm_err_flag_type 
pm_pon_set_spare_reg (uint32 pmic_device_index,pm_pon_spare_reg_type spare_reg, boolean set_value, uint8 mask);



/*!
 * @brief  This function returns the phone uvlo reset status. Used in boot
 *         to determine if we should go into DLOAD mode.
 *
 * INPUT PARAMETERS
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param status
 *    - pointer to boolean that indicates if we reset due to UVLO reset
 *
 * @return pm_err_flag_type.
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                       version of the PMIC.
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 *
 * @par Dependencies
 *      and pm_init() must have been called.
*/
pm_err_flag_type 
pm_pon_uvlo_reset_status(uint32 pmic_device_index, boolean * status);


/**
* @brief This function allows the user to read the PON trigger status
*
* @param[in]  pmic_device_index: Device index of PMIC chip. 0 for primary.
*                   
* @param[in]  trigger: PON trigger. Refer enum#pm_pon_trigger_type for more info
*  
* @param[out] status: PON trigger enabled/disabled status  
*                   
*
* @return PM_ERR_FLAG_SUCCESS               = The operation was successful.
*          PM_ERR_FLAG_INVALID_PARAMETER     = Parameter 1 is out of range.
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = This feature not supported
*          PM_ERR_FLAG_BUS_ERR           = The SPMI driver failed to
*                                               communicate with the PMIC.
*
*/
pm_err_flag_type 
pm_pon_trigger_enable_status(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean *status);

/**
* @brief This function allows the user to enable/disable power on trigger
*       
*
* @param[in]  pmic_device_index: Device index of PMIC chip. 0 for primary, 1 for secondary and so on.
* @param[in]  trigger: PON trigger. Refer enum#pm_pon_trigger_type for more info
* @param[in]  enable:  Enable/disable trigger for PON
*
* @return PM_ERR_FLAG_SUCCESS                = The operation was successful.
*          PM_ERR_FLAG_INVALID_PARAMETER     = Parameter 1 is out of range.
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not supported
*          PM_ERR_FLAG_BUS_ERR           = The SPMI driver failed to
*                                               communicate with the PMIC.
*
*/
pm_err_flag_type 
pm_pon_trigger_enable(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean enable);

/**
 * @brief Configures PMIC to act on MSM PS_HOLD toggle. This
 *        is an app level API which handles all the required
 *        PS_HOLD config for all the applicable PMICs
 *        internally.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state. 
 *  
 * @param app_pshold_cfg : Supported configurations: 
 *                               PM_APP_PON_CFG_WARM_RESET,
 *                               PM_APP_PON_CFG_HARD_RESET,
 *                               PM_APP_PON_CFG_NORMAL_SHUTDOWN
 *   
 *                   
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type app_pshold_cfg);


/**
 * @brief Configures PMIC Reset. This is an app level API which 
 *        handles PON reset source and reset type for all the
 *        applicable PMICs internally.
 *  
 * @details Configure PMIC reset source and reset type.
 *  
 * @param app_pon_reset_source  :   pm_app_pon_reset_source_type 
 *  
 *@param app_pon_reset_cfg : Supported configurations: 
 *                               PM_APP_PON_CFG_DVDD_HARD_RESET 
 *                               PM_APP_PON_CFG_WARM_RESET,
 *   
 *                   
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_app_pon_reset_cfg(pm_app_pon_reset_source_type app_pon_reset_source, pm_app_pon_reset_cfg_type app_pon_reset_cfg, uint32 s1_timer, uint32 s2_timer);

/**
 * @brief Configures PMIC Reset. This is an app level API which 
 *        handles PON reset source and reset type for all the
 *        applicable PMICs internally.
 *  
 * @details Configure PMIC PON s3 reset timer for various reset
 *          sources.  It takes the s3 timer for primary pmic and
 *          and it automatically configures all existing PMICs
 *          using pre selected multiplier.
 *  
 * @param app_pon_reset_source :   pm_app_pon_reset_source_type 
 *  
 *@param s3_timer :Range of configuration for primary pmic: 
 *               {0, 2, 4, 8, 16, 32, 64, 128}
 *   
 *                   
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_app_pon_s3_reset_timer_cfg(pm_app_pon_reset_source_type app_pon_s3_reset_source, uint32 s3_timer );


/**
 * @brief Log PMICs PON reasons
 *  
 * @details Detects availabel PMICs and log PON Reason to UART log window
 *  
 * @param pon_verbose_log_mode 
 *          TRUE:  Verbose PON Reason of each PMIC gets logged to UART
 *          FALSE: Minimal PON Reason of each PMIC gets logged to UART
                    
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type 
pm_log_pon_reasons(boolean pon_verbose_log_mode);


//Applicable only for PON GEN2 
/**
* @brief This function gests the current state of FSM state machine
*       
*
* @param[in]  pmic_device_index: Device index of PMIC chip. 0 for primary, 1 for secondary and so on.
* @param[in]  pon_fsm_status: pm_pon_fsm_status_type
*
* @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
*/
pm_err_flag_type
pm_pon_get_fsm_current_state(uint8 pmic_device_index, pm_pon_fsm_status_type *pon_fsm_status);


/**
* @brief This function configures option bit registers
*       
*
* @param[in]  pmic_device_index: Device index of PMIC chip. 0 for primary 
*             ONLY PRIMARY PMIC is allowed 
* @param[in]  cfg_field: pm_pon_option_bit_type - refer pm_pon.h for more info
 
* @param[in]  cfg_data: TRUE  : Falling/Rising edge in power on sequence OFF state
*                       FALSE : High/LOW level 
*
* @return PM_ERR_FLAG_SUCCESS                = The operation was successful.
*          PM_ERR_FLAG_INVALID_PARAMETER     = Parameter 1 is out of range.
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not supported
*          PM_ERR_FLAG_BUS_ERR               = The SPMI driver failed to
*                                               communicate with the PMIC.
*
*/
pm_err_flag_type 
pm_pon_set_option_bit(uint32 pmic_device_index, pm_pon_option_bit_type cfg_field, boolean cfg_data);

#endif // __PM_PON_H__
