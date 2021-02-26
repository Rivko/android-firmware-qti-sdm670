/*! \file pm_pon.c
*  \n
*  \brief PMIC PowerON Peripheral driver
*  \n
*  \n This header file contains class implementation of the PON
*  \n peripheral APIs
*  \n
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
			        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/30/17   st      Corrected PON reason reading
08/22/17   czq     Support for PON_RESET_CFG_WARM_RESET_THEN_DVDD_SHUTDOWN_VAL.
05/09/17   czq     correct some faults in API pm_pon_gen1_config_uvlo(). 
01/06/17   al      Updated pon_init for pon_gen2 warm reset
11/11/16   aab     Updated pon_gen2_uvlo_threshold_val[]
05/06/16   aab     Updated pm_pon_overtemp_reset_cf() and pm_pon_get_reset_config_val()
05/02/16   aab     Support for RUMI with no pmic card.
03/28/16   aab     Added pm_pon_gp_reset_cfg() and pm_pon_get_reset_config_val()
03/22/16   aab     Added pm_pon_get_fault_reasons() updated pm_pon_get_all_pon_reasons() 
03/22/16   aab     Updated pm_pon_config_uvlo() Added pm_pon_gen1_config_uvlo()
12/07/15   aab     Updated to support PON GEN2 peripheral
09/16/15   al      Updating xvdd status read 
08/08/15   aab     Updated pm_pon_init() to support PON APIs with out doing pm init
11/12/15   kt      Updated pm_pon_ps_hold_cfg API.
11/19/14   al      Removing loader specific 
09/15/14   aab     Added PON Reasons status log
07/16/14   akm     Comm change Updates
08/19/14   al      Adding API to configure PON trigger
12/04/13   aab     Added PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET
11/01/13   sv      Applying CR538994 Changes with power up failure fix. (CR-570053)
10/24/13   sv      Reverting CR538994 Changes (CR-566323)
06/24/13   ps      Added PON Stage3 reset config API CR#495834 
02/27/13   kt      Added PON Reasons and IRQ related APIs  
01/24/13   umr     Add new version support.
11/20/12   umr     Created
========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "pm_version.h"
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pmio_pon.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_config_target.h"
#include "pm_device.h"
#include "pm_sbl_boot.h"




/*===========================================================================
                 #defines 
===========================================================================*/

/* PMIC Device Index IDs */
#define PMIC_A_DEVICE_ID   0
#define PMIC_B_DEVICE_ID   1

#define PON_RESET_CFG_INVALID_VAL                   0xFF

/* PON_DVDD_RB_SPARE reg Warm Reset indicating bit mask */
#define PON_DVDD_RB_SPARE_WARM_RESET_MASK                 0x01
#define PM_PON_MAX_SPARE_BIT_MASK                         0xFF
#define PMIO_PON_TRIGGER_EDGE_CONFIG                      0x881

/* length of array */
#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof(x[0]))

#define WAIT_FOR_SLEEP_CLK_CYCLE(num_of_sleep_cycle)  pm_busy_wait((3052*num_of_sleep_cycle)/100) //sleep cycle is 32.7645KHz

/*===========================================================================
                 local global variable 
===========================================================================*/

static boolean pm_pon_initialized = FALSE;

typedef struct 
{
  uint16 s1_timer_addr; 
  uint16 s1_timer_mask; 
  uint16 s2_timer_addr;
  uint16 s2_timer_mask;
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
}pon_reset_reg_info_type;

typedef struct
{
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
}pon_ps_hold_reg_info_type;

typedef struct
{
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
}pon_overtemp_reg_info_type;

typedef struct
{
  uint16 ctl_addr;
  uint16 kpwpwr_mask;
  uint16 cblpwr_mask;
  uint16 pon1_mask;
  uint16 usb_chg_mask;
  uint16 dc_chg_mask;
  uint16 rtc_mask;
  uint16 smpl_mask;
}pon_trigger_en_reg_info_type;

typedef struct
{
  uint8 pon_reason1;
  uint8 pon_reason2;
  uint8 poff_reason1;
  uint8 poff_reason2;
  uint8 warm_reset_reason1;
  uint8 warm_reset_reason2;
  uint8 soft_reset_reason1;
  uint8 soft_reset_reason2;
}pm_reason_status_type;

typedef struct
{
  uint8 mask;
  uint8 shift;
}pm_pon_reg_mask_shift_info_type;


static pon_trigger_en_reg_info_type
  pon_trigger_en_reg =
{
  PMIO_PON_PON_TRIGGER_EN_ADDR,
  PMIO_PON_PON_TRIGGER_EN_KPDPWR_N_BMSK,
  PMIO_PON_PON_TRIGGER_EN_CBLPWR_N_BMSK,
  PMIO_PON_PON_TRIGGER_EN_PON1_BMSK,
  PMIO_PON_PON_TRIGGER_EN_USB_CHG_BMSK,
  PMIO_PON_PON_TRIGGER_EN_DC_CHG_BMSK,
  PMIO_PON_PON_TRIGGER_EN_RTC_BMSK,
  PMIO_PON_PON_TRIGGER_EN_SMPL_BMSK,
};

static pon_reset_reg_info_type
  pon_kpdpwr_reg =
{
  PMIO_PON_KPDPWR_N_RESET_S1_TIMER_ADDR,
  PMIO_PON_KPDPWR_N_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_KPDPWR_N_RESET_S2_TIMER_ADDR,
  PMIO_PON_KPDPWR_N_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_KPDPWR_N_RESET_S2_CTL_ADDR,
  PMIO_PON_KPDPWR_N_RESET_S2_CTL2_ADDR,
  PMIO_PON_KPDPWR_N_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_KPDPWR_N_RESET_S2_CTL_RESET_TYPE_BMSK
};

static pon_reset_reg_info_type
  pon_resin_reg =
{
  PMIO_PON_RESIN_N_RESET_S1_TIMER_ADDR,
  PMIO_PON_RESIN_N_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_RESIN_N_RESET_S2_TIMER_ADDR,
  PMIO_PON_RESIN_N_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_RESIN_N_RESET_S2_CTL_ADDR,
  PMIO_PON_RESIN_N_RESET_S2_CTL2_ADDR,
  PMIO_PON_RESIN_N_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_RESIN_N_RESET_S2_CTL_RESET_TYPE_BMSK
};

static pon_reset_reg_info_type
  pon_resin_and_kpdpwr_reg =
{
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_ADDR,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_ADDR,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_ADDR,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_ADDR,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_RESIN_AND_KPDPWR_RESET_S2_CTL_RESET_TYPE_BMSK
};

static pon_reset_reg_info_type
  pon_gp1_reg =
{
  PMIO_PON_GP1_RESET_S1_TIMER_ADDR,
  PMIO_PON_GP1_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_GP1_RESET_S2_TIMER_ADDR,
  PMIO_PON_GP1_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_GP1_RESET_S2_CTL_ADDR,
  PMIO_PON_GP1_RESET_S2_CTL2_ADDR,
  PMIO_PON_GP1_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_GP1_RESET_S2_CTL_RESET_TYPE_BMSK
};

static pon_reset_reg_info_type
  pon_gp2_reg =
{
  PMIO_PON_GP2_RESET_S1_TIMER_ADDR,
  PMIO_PON_GP2_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_GP2_RESET_S2_TIMER_ADDR,
  PMIO_PON_GP2_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_GP2_RESET_S2_CTL_ADDR,
  PMIO_PON_GP2_RESET_S2_CTL2_ADDR,
  PMIO_PON_GP2_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_GP2_RESET_S2_CTL_RESET_TYPE_BMSK
};

static pon_ps_hold_reg_info_type
  pon_ps_hold_reg =
{
  PMIO_PON_PS_HOLD_RESET_CTL_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_ADDR,
  PMIO_PON_PS_HOLD_RESET_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_PS_HOLD_RESET_CTL_RESET_TYPE_BMSK
};

static pm_register_data_type pm_pon_trigger_mask[] =
{
  [PM_PON_TRIGGER_KPDPWR]  = PMIO_PON_PON_TRIGGER_EN_KPDPWR_N_BMSK,
  [PM_PON_TRIGGER_CBLPWR] = PMIO_PON_PON_TRIGGER_EN_CBLPWR_N_BMSK,
  [PM_PON_TRIGGER_PON1]    = PMIO_PON_PON_TRIGGER_EN_PON1_BMSK,
  [PM_PON_TRIGGER_USB_CHG] = PMIO_PON_PON_TRIGGER_EN_USB_CHG_BMSK,
  [PM_PON_TRIGGER_DC_CHG]  = PMIO_PON_PON_TRIGGER_EN_DC_CHG_BMSK,
  [PM_PON_TRIGGER_RTC] = PMIO_PON_PON_TRIGGER_EN_RTC_BMSK,
  [PM_PON_TRIGGER_SMPL]    = PMIO_PON_PON_TRIGGER_EN_SMPL_BMSK,
};

static pm_register_data_type pon_reset_cfg_val[] =
{
  [PM_PON_RESET_CFG_WARM_RESET]                                         = 0x01,
  [PM_PON_RESET_CFG_HARD_RESET]                                         = 0x07,
  [PM_PON_RESET_CFG_NORMAL_SHUTDOWN]                                    = 0x04,
  [PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET]                       = 0x08,
  [PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN]                         = 0x05,
  [PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN]                    = 0x06,
  [PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET]                  = 0x09,
  [PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN]          = 0x02,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN]         = 0x0A,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN] = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN]                           = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET] = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET]       = PON_RESET_CFG_INVALID_VAL,
  [PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET] = PON_RESET_CFG_INVALID_VAL,
};

static pm_register_address_type pon_spare_register_addr[] =
{
  [PM_PON_PERPH_SPARE] = PMIO_PON_PERPH_RB_SPARE_ADDR,
  [PM_PON_DVDD_SPARE] = PMIO_PON_DVDD_RB_SPARE_ADDR,
  [PM_PON_XVDD_SPARE]  = PMIO_PON_XVDD_RB_SPARE_ADDR,
  [PM_PON_SOFT_SPARE] = PMIO_PON_SOFT_RB_SPARE_ADDR,
};

static pon_reset_reg_info_type *pon_reset_reg[] =
{
  [PM_PON_RESET_SOURCE_KPDPWR]           = &pon_kpdpwr_reg,
  [PM_PON_RESET_SOURCE_RESIN] = &pon_resin_reg,
  [PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR] = &pon_resin_and_kpdpwr_reg,
  [PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR] = NULL,
  [PM_PON_RESET_SOURCE_GP1]              = &pon_gp1_reg,
  [PM_PON_RESET_SOURCE_GP2] = &pon_gp2_reg,
};

static pm_pon_reg_mask_shift_info_type option_bit[] =
{
  [PM_PON_OPTION_DC_CHG_REDGE_PON] = { PMIO_PON_OPTION_BIT_DC_CHG_REDGE_PON_BMSK,   PMIO_PON_OPTION_BIT_DC_CHG_REDGE_PON_SHFT },
  [PM_PON_OPTION_USB_CHG_REDGE_PON] = { PMIO_PON_OPTION_BIT_USB_CHG_REDGE_PON_BMSK,  PMIO_PON_OPTION_BIT_USB_CHG_REDGE_PON_SHFT },
  [PM_PON_OPTION_CBLPWR_FEDGE_PON] = { PMIO_PON_OPTION_BIT_CBLPWR_FEDGE_PON_BMSK,   PMIO_PON_OPTION_BIT_CBLPWR_FEDGE_PON_SHFT },
  [PM_PON_OPTION_PON1_REDGE_PON]   = { PMIO_PON_OPTION_BIT_PON1_REDGE_PON_BMSK,     PMIO_PON_OPTION_BIT_PON1_REDGE_PON_SHFT },
  [PM_PON_OPTION_KPDPWR_FEDGE_PON] = { PMIO_PON_OPTION_BIT_KPDPWR_FEDGE_PON_BMSK,   PMIO_PON_OPTION_BIT_KPDPWR_FEDGE_PON_SHFT },
};

static uint32
pon_s1_timer_val[] =
{ 0, 32, 56, 80, 128, 184, 272, 408, 608, 904, 1352, 2048, 3072, 4480, 6720, 10256 };

static uint32
pon_s2_timer_val[] =
{ 0, 10, 50, 100, 250, 500, 1000, 2000 };

static uint32
pon_s3_timer_val[] =
{ 0, 2, 4, 8, 16, 32, 64, 128 };

static uint32
pon_debounce_time_val[] =
{ 60, 120, 240, 490, 980, 2000, 3900, 7800, 15600, 31200, 62500, 125000, 25000 };

static uint32
pon_uvlo_hyst_val[] =
{ 150, 300, 450, 600 };

static uint32
pon_uvlo_threshold_val[] =
{ 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000, 2050, 2100, 2150, 2200, 2250, 2300,
  2350, 2400, 2450, 2500, 2550, 2600, 2650, 2700, 2750, 2800, 2850, 2900, 2950, 3000, 3050 };

typedef struct
{
  peripheral_info_type periph_info;
}pm_pon_data_type;

static pm_pon_data_type pm_pon_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================
                  Function Prototypes
===========================================================================*/


/*===========================================================================
                  Local Function
===========================================================================*/

/*internal function to return slave id*/
static pm_err_flag_type pm_pon_get_pon_info(uint8 pmic_device_index, uint32 *pon_slave_id)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if (NULL == pon_slave_id)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (pm_pon_initialized == FALSE)
  {
    err_flag = pm_pon_init();
    if (pm_pon_initialized != TRUE)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
  }

  err_flag = pm_get_slave_id(pmic_device_index, 0, pon_slave_id);

  return err_flag;
}

/*===========================================================================

API Implementation

===========================================================================*/

pm_err_flag_type
pm_pon_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8   pmic_index  = 0;
  uint8   sid_index   = 0;
  uint8   periph_type = 0;

  peripheral_info_type  pon_peripheral_info;
  uint8   warm_reset_reason[2] = { 0 };

  if (pm_pon_initialized == TRUE)
  {
    return err_flag;
  }

  memset(&pon_peripheral_info, 0, sizeof(peripheral_info_type));
  pon_peripheral_info.base_address = PON_BASE;

  /* i.Ensure that pmic device is initialized ii.check for vaid PON peripheral */
  err_flag = pm_device_setup();

  err_flag |= pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_TYPE_ADDR, &periph_type, 0);
  if ((periph_type != PM_HW_MODULE_PON) || (err_flag != PM_ERR_FLAG_SUCCESS))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Get PON perepheral type and perepheral sub type for each valid PMIC on target */
  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    sid_index = pmic_index * 2;
    if (pm_get_peripheral_info(sid_index, &pon_peripheral_info) == PM_ERR_FLAG_SUCCESS)
    {
      pm_pon_data_arr[pmic_index].periph_info = pon_peripheral_info;
    }
  }

  /* Read ON reason and write the DVDD_RB_SPARE register bit <0> to 1 if Warm Reset occured */
  err_flag |= pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_GEN2_ON_REASON_ADDR, warm_reset_reason, 0);
  if (warm_reset_reason[0] & PMIO_PON_GEN2_ON_REASON_WARM_SEQ_BMSK)
  {
    err_flag |= pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_DVDD_RB_SPARE_ADDR, PON_DVDD_RB_SPARE_WARM_RESET_MASK, 0x01, 0);
  }

  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    pm_pon_initialized = TRUE;
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_ps_hold_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 ps_hold_cfg_val = 0;
  uint32 slave_id = 0;

  if (ps_hold_cfg >= PM_PON_RESET_CFG_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if (pon_reset_cfg_val[ps_hold_cfg] == PON_RESET_CFG_INVALID_VAL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      ps_hold_cfg_val = pon_reset_cfg_val[ps_hold_cfg];

      /* i.) Disable reset ii.) wait for 10 sleep clock cycles iii.)config and iv.) enable reset */
      err_flag = pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl2_addr, 0x0, 0);
      WAIT_FOR_SLEEP_CLK_CYCLE(10);
      err_flag |= pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl_addr, ps_hold_cfg_val, 0);
      err_flag |= pm_comm_write_byte(slave_id, pon_ps_hold_reg.reset_ctl2_addr, pon_ps_hold_reg.reset_en_mask, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_reset_source_cfg(uint32 pmic_device_index, pm_pon_reset_source_type reset_source,
                        uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 reset_en, reset_val = 0;
  uint8 s1_timer_id = 0;
  uint8 s2_timer_id = 0;
  uint32 slave_id = 0;
  pon_reset_reg_info_type *reset_reg = NULL;

  if (reset_cfg >= PM_PON_RESET_CFG_INVALID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  reset_reg = pon_reset_reg[reset_source];
  if (reset_reg == NULL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if(err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /*read current enabled status*/
  err_flag = pm_comm_read_byte(slave_id, reset_reg->reset_ctl2_addr, &reset_en, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /*if already enabled then disable and wait for 10 sleep clock cycle*/
  if (reset_en & reset_reg->reset_en_mask)
  {
    err_flag = pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl2_addr, reset_reg->reset_en_mask, 0x0, 0);
    WAIT_FOR_SLEEP_CLK_CYCLE(10);
  }

  /* If reset source is keypress then disable keypress pon trigger before configuring S1 and S2 and re-enable it*/
  if (reset_source == PM_PON_RESET_SOURCE_KPDPWR)
  {
    err_flag |= pm_comm_write_byte_mask(slave_id, pon_trigger_en_reg.ctl_addr, pon_trigger_en_reg.kpwpwr_mask, 0x0, 0);
    WAIT_FOR_SLEEP_CLK_CYCLE(5);
  }

  /* Calc and configure S1 and S2 timer. Get the upper limit if a value falls in between */
  s1_timer_id = pm_utils_get_upper_idx(s1_timer, pon_s1_timer_val, ARRAY_LENGTH(pon_s1_timer_val));
  s2_timer_id = pm_utils_get_upper_idx(s2_timer, pon_s2_timer_val, ARRAY_LENGTH(pon_s2_timer_val));
  err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->s1_timer_addr, reset_reg->s1_timer_mask, s1_timer_id, 0);
  err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->s2_timer_addr, reset_reg->s2_timer_mask, s2_timer_id, 0);

  if (reset_source == PM_PON_RESET_SOURCE_KPDPWR)
  {
    err_flag |= pm_comm_write_byte_mask(slave_id, pon_trigger_en_reg.ctl_addr, pon_trigger_en_reg.kpwpwr_mask, 0xFF, 0);
  }

  /* Configure reset value */
  reset_val = pon_reset_cfg_val[reset_cfg];
  if (reset_val < PON_RESET_CFG_INVALID_VAL)
  {
    err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl_addr, reset_reg->reset_type_mask, reset_val, 0);
  }
  else
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  /* Enable reset if it was already in enable state */
  if (reset_en & reset_reg->reset_en_mask)
  {
    err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl2_addr, reset_reg->reset_en_mask, 0xFF, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_gp_reset_cfg(uint32  pmic_device_index, pm_pon_reset_source_type reset_source, pm_pon_reset_cfg_type reset_cfg_type)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 reset_en = 0;
  uint8 reset_val = 0;
  uint32 slave_id = 0;
  pon_reset_reg_info_type *reset_reg = NULL;

  if (reset_cfg_type >= PM_PON_RESET_CFG_INVALID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (reset_source != PM_PON_RESET_SOURCE_GP1
      && reset_source != PM_PON_RESET_SOURCE_GP2)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  reset_reg = pon_reset_reg[reset_source];
  if (NULL == reset_reg)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Read reset status */
  err_flag |= pm_comm_read_byte(slave_id, reset_reg->reset_ctl2_addr, &reset_en, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  if (reset_en & reset_reg->reset_en_mask)
  {
    /* disable reset and wait for 8 sleep clock cycles of delay */
    err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl2_addr, reset_reg->reset_en_mask, 0x0, 0);
    WAIT_FOR_SLEEP_CLK_CYCLE(8);
  }

  reset_val = pon_reset_cfg_val[reset_cfg_type];
  if (reset_val == PON_RESET_CFG_INVALID_VAL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  /* Configure reset value */
  err_flag = pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl_addr, reset_reg->reset_type_mask, reset_val, 0);

  if (reset_en & reset_reg->reset_en_mask)
  {  /* Enable reset if it was already in enable state */
    err_flag |= pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl2_addr, reset_reg->reset_en_mask, 0xFF, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_reset_source_ctl(uint32 pmic_device_index, pm_pon_reset_source_type reset_source, pm_on_off_type on_off)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;
  pon_reset_reg_info_type *reset_reg = NULL;
  uint8 data = (on_off == PM_ON) ? 0xFF : 0;

  reset_reg = pon_reset_reg[reset_source];

  if (NULL == reset_reg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_write_byte_mask(slave_id, reset_reg->reset_ctl2_addr, reset_reg->reset_en_mask, data, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_stage3_reset_source_cfg(uint32 pmic_device_index, pm_pon_reset_source_type reset_source, uint32 s3_timer)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 s3_timer_id = 0;
  uint32 slave_id = 0;

  if (reset_source > PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Write to Set Stage 3 reset source, write once register */
  err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_RESET_S3_SRC_ADDR, PMIO_PON_RESET_S3_SRC_RMSK, reset_source, 0);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    /* Calc S3 timer id, unlock secure register and set stage 3 reset timer*/
    s3_timer_id = pm_utils_get_upper_idx(s3_timer, pon_s3_timer_val, ARRAY_LENGTH(pon_s3_timer_val));
    err_flag = pm_comm_write_byte(slave_id, PMIO_PON_SEC_ACCESS_ADDR, 0xA5, 0);
    err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_RESET_S3_TIMER_ADDR, PMIO_PON_RESET_S3_TIMER_RMSK, s3_timer_id, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_fsm_current_state(uint8 pmic_device_index, pm_pon_fsm_status_type *pon_fsm_status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type fsm_status_val = 0x00;
  uint32 slave_id = 0;

  if (pon_fsm_status == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_GEN2_FSM_STATUS_ADDR, &fsm_status_val, 0);
      *pon_fsm_status = (pm_pon_fsm_status_type)(fsm_status_val & PMIO_PON_GEN2_FSM_STATUS_BMSK);
    }
  }

  return err_flag;
}

pm_err_flag_type
pm_pon_get_pon_reason(uint32 pmic_device_index, pm_pon_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_GEN2_PON_REASON1_ADDR, (uint8 *)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_poff_reason(uint32 pmic_device_index, pm_pon_poff_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, PMIO_PON_GEN2_POFF_REASON1_ADDR, 2, (uint8 *)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_warm_reset_reason(uint32 pmic_device_index, pm_pon_warm_reset_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, PMIO_PON_GEN2_WARM_RESET_REASON1_ADDR, 1, (uint8 *)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_on_reason(uint32 pmic_device_index, pm_pon_on_state_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_GEN2_ON_REASON_ADDR, (uint8 *)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_off_reason(uint32 pmic_device_index, pm_pon_off_state_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_GEN2_OFF_REASON_ADDR, (uint8*)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_stage3_reset_reason(uint32 pmic_device_index, pm_pon_stage3_reset_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_GEN2_S3_RESET_REASON_ADDR, (uint8 *)reason, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_all_pon_reasons(uint32 pmic_device_index, uint64 *pon_reasons)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;
  
  if (pon_reasons == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, PMIO_PON_GEN2_PON_REASON1_ADDR, 8, (uint8 *)pon_reasons, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_fault_reason(uint32 pmic_device_index, pm_pon_fault_state_reason_type *reason)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (reason == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, PMIO_PON_GEN2_FAULT_REASON1_ADDR, 2, (uint8*)reason, 0);
    }
  }

  return err_flag;
}

pm_err_flag_type
pm_pon_get_fault_reasons(uint32 pmic_device_index, uint64 *fault_reasons)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  if (fault_reasons == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte_array(slave_id, PMIO_PON_GEN2_FAULT_REASON1_ADDR, 4, (uint8 *)fault_reasons, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_debounce_ctl(uint32 pmic_device_index, uint32 time_delay)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  uint32 slave_id = 0;
  uint8 time_id = pm_utils_get_upper_idx(time_delay, pon_debounce_time_val,
                                         ARRAY_LENGTH(pon_debounce_time_val));

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_DEBOUNCE_CTL_ADDR,
                                       PMIO_PON_GEN2_DEBOUNCE_CTL_DEBOUNCE_BMSK, time_id, 0);
    /* Wait for 5 sleep clock cycle */
    WAIT_FOR_SLEEP_CLK_CYCLE(5);
  }

  return err_flag;
}

pm_err_flag_type
pm_pon_irq_enable(uint32 pmic_device_index, pm_pon_irq_type irq, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 1 << irq;
  pm_register_address_type irq_reg = enable ? PMIO_PON_INT_EN_SET_ADDR : PMIO_PON_INT_EN_CLR_ADDR;

  uint32 slave_id = 0;

  if (irq >= PM_PON_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_write_byte(slave_id, irq_reg, mask, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_irq_clear(uint32  pmic_device_index, pm_pon_irq_type irq)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1 << irq;

  if (irq >= PM_PON_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_write_byte(slave_id, PMIO_PON_INT_LATCHED_CLR_ADDR, mask, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_irq_set_trigger(uint32 pmic_device_index, pm_pon_irq_type irq, pm_irq_trigger_type trigger)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1 << irq;
  uint8 set_type = 0;
  uint8 polarity_high = 0;
  uint8 polarity_low = 0;

  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  switch (trigger)
  {
  case PM_IRQ_TRIGGER_ACTIVE_LOW:
    set_type = 0x00;
    polarity_high = 0x00;
    polarity_low = 0xFF;
    break;
  case PM_IRQ_TRIGGER_ACTIVE_HIGH:
    set_type = 0x00;
    polarity_high = 0xFF;
    polarity_low = 0x00;
    break;
  case PM_IRQ_TRIGGER_RISING_EDGE:
    set_type = 0xFF;
    polarity_high = 0xFF;
    polarity_low = 0x00;
    break;
  case PM_IRQ_TRIGGER_FALLING_EDGE:
    set_type = 0xFF;
    polarity_high = 0x00;
    polarity_low = 0xFF;
    break;
  case PM_IRQ_TRIGGER_DUAL_EDGE:
    set_type = 0xFF;
    polarity_high = 0xFF;
    polarity_low = 0xFF;
    break;
  default:
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_SET_TYPE_ADDR, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_POLARITY_HIGH_ADDR, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_INT_POLARITY_LOW_ADDR, mask, polarity_low, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_irq_status(uint32 pmic_device_index, pm_pon_irq_type irq, pm_irq_status_type type, boolean *status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;
  uint8 mask = 1 << irq;
  uint8 data = 0;
  pm_register_address_type reg = 0;

  if (status == NULL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (irq >= PM_PON_IRQ_INVALID)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  switch (type)
  {
  case PM_IRQ_STATUS_RT:
    reg = PMIO_PON_INT_RT_STS_ADDR;
    break;
  case PM_IRQ_STATUS_LATCHED:
    reg = PMIO_PON_INT_LATCHED_STS_ADDR;
    break;
  case PM_IRQ_STATUS_PENDING:
    reg = PMIO_PON_INT_PENDING_STS_ADDR;
    break;
  default:
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    err_flag = pm_comm_read_byte(slave_id, reg, &data, 0);
    *status = (data & mask) ? TRUE : FALSE;
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_config_uvlo(uint32 pmic_device_index, uint32 uvlo_thresh_voltage_in_mv, uint32 uvlo_hyst_voltage_in_mv, boolean uvlo_enable)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 uvlo_thresh_data = 0;
  uint8 uvlo_hyst_data   = 0;
  uint8 uvlo_ctl_data = 0;
  uint32 slave_id = 0;

  //Calculate UVLO HYST
  uvlo_hyst_data = pm_utils_get_upper_idx(uvlo_hyst_voltage_in_mv, pon_uvlo_hyst_val, ARRAY_LENGTH(pon_uvlo_hyst_val));
  uvlo_thresh_data = pm_utils_get_upper_idx(uvlo_thresh_voltage_in_mv, pon_uvlo_threshold_val, ARRAY_LENGTH(pon_uvlo_threshold_val));

  uvlo_ctl_data = (uvlo_enable << PMIO_PON_UVLO_ENABLE_PON_SHIFT) | (uvlo_hyst_data << PMIO_PON_UVLO_HYST_DATA_PON_SHIFT) | uvlo_thresh_data;


  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    //Config UVLO CTL
    err_flag = pm_comm_write_byte(slave_id, PMIO_PON_SEC_ACCESS_ADDR, 0xA5, 0);
    err_flag |= pm_comm_write_byte(slave_id, PMIO_PON_UVLO_CTL, uvlo_ctl_data, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_warm_reset_status(uint32 pmic_device_index, boolean *status)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8              data = 0;
  uint32 slave_id = 0;

  if (status == NULL)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      /*spare register stores warm reset info*/
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_DVDD_RB_SPARE_ADDR, &data, 0);
      *status = (data & PON_DVDD_RB_SPARE_WARM_RESET_MASK) ? TRUE : FALSE;
    }
  }

  return err_flag;
}

pm_err_flag_type
pm_pon_warm_reset_status_clear(uint32 pmic_device_index)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint32 slave_id = 0;

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_DVDD_RB_SPARE_ADDR, PON_DVDD_RB_SPARE_WARM_RESET_MASK, 0x00, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_pbl_get_status(uint32  pmic_device_index, pm_pon_pbl_status_type pbl_status, boolean *status)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type  data = 0;
  uint32 slave_id = 0;

  if (status == NULL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  err_flag = pm_comm_read_byte(slave_id, PMIO_PON_PON_PBL_STATUS_ADDR, &data, 0);

  if (pbl_status == PM_PON_PBL_DVDD_RB_OCCURRED)
  {
    *status = (data & PMIO_PON_PON_PBL_STATUS_DVDD_RB_OCCURRED_BMSK) ? TRUE : FALSE;
  }
  else if (pbl_status == PM_PON_PBL_XVDD_RB_OCCURRED)
  {
    *status = (data & PMIO_PON_PON_PBL_STATUS_XVDD_RB_OCCURRED_BMSK) ? TRUE : FALSE;
  }
  else
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_get_spare_reg(uint32 pmic_device_index, pm_pon_spare_reg_type spare_reg, uint8 *data_ptr)
{
  pm_err_flag_type	 err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type  spare_register_addr =  0x00;
  uint32 slave_id = 0;

  if ((spare_reg >= PM_PON_SPARE_INVALID) || (!data_ptr))
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      spare_register_addr = pon_spare_register_addr[spare_reg];
      err_flag = pm_comm_read_byte(slave_id, spare_register_addr, data_ptr, 0);
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_set_spare_reg(uint32 pmic_device_index, pm_pon_spare_reg_type spare_reg, uint8 set_value, uint8 mask)
{
  pm_err_flag_type	 err_flag = PM_ERR_FLAG_SUCCESS;

  pm_register_address_type  spare_register_addr;

  uint32 slave_id = 0;

  if ((spare_reg >= PM_PON_SPARE_INVALID)
      || (mask > PM_PON_MAX_SPARE_BIT_MASK))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
  if (err_flag == PM_ERR_FLAG_SUCCESS)
  {
    spare_register_addr = pon_spare_register_addr[spare_reg];
    err_flag = pm_comm_write_byte_mask(slave_id, spare_register_addr, mask, set_value, 0);
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_trigger_enable_status(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean *status)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 pon_trigger;

  uint32 slave_id = 0;

  if ((trigger >= PM_PON_TRIGGER_INVALID) || (NULL == status))
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      err_flag = pm_comm_read_byte(slave_id, PMIO_PON_PON_TRIGGER_EN_ADDR, &pon_trigger, 0);
      *status =  (pon_trigger & pm_pon_trigger_mask[trigger]) ? TRUE : FALSE;
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_pon_trigger_enable(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean enable)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 0;
  uint8 data = (enable) ? 0xFF : 0x00;
  uint32 slave_id = 0;

  if (trigger >= PM_PON_TRIGGER_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      mask = pm_pon_trigger_mask[trigger];
      err_flag = pm_comm_write_byte_mask(slave_id, PMIO_PON_PON_TRIGGER_EN_ADDR, mask, data, 0);
      /* Requires 3 sleep clock cycles of delay */
      WAIT_FOR_SLEEP_CLK_CYCLE(3);
    }
  }

  return err_flag;
}


pm_err_flag_type pm_pon_set_option_bit(uint32 pmic_device_index, pm_pon_option_bit_type cfg_field, boolean cfg_data)
{
  pm_err_flag_type   err_flag       = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data   = (cfg_data) ? 0x01 : 0x00;
  pm_register_data_type write_data  = 0;
  pm_register_data_type mask   = 0;
  pm_register_data_type read_data = 0;
  uint32 slave_id = 0;

  if (cfg_field >= PM_PON_OPTION_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = pm_pon_get_pon_info(pmic_device_index, &slave_id);
    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      write_data = data << option_bit[cfg_field].shift;
      mask = option_bit[cfg_field].mask;

      err_flag |= pm_comm_write_byte_mask(slave_id, PMIO_PON_OPTION_BIT_ADDR, mask, write_data, 0);
      //Read the data from 0x882 and Write it to 0x881
      err_flag |= pm_comm_read_byte(slave_id, PMIO_PON_OPTION_BIT_ADDR, &read_data, 0);
      err_flag |= pm_comm_write_byte(slave_id, PMIO_PON_TRIGGER_EDGE_CONFIG, read_data, 0);
    }
  }

  return err_flag;
}


