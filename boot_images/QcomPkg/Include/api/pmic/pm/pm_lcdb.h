#ifndef PM_LCDB_H
#define PM_LCDB_H

/*! \file pm_lcdb.h
 *  \n
 *  \brief  PMIC LCDB MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC LCDB interface module.
 *
 *  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/15/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPEDEF AND MACRO DEFINITIONS

===========================================================================*/

typedef struct
{
    /* LCDB Periph Status (overall) - 0xEC08, 0xEC0C */
    boolean  lcdb_sc_det;
    boolean  lcdb_vreg_ok;
    uint8    lcdb_seq_status;
    /* BOOST Module Status - 0xEC09, 0xEC0D */
    boolean  boost_vreg_ok;
    boolean  boost_sc_det;
    uint8    boost_status;
    uint32   boost_output_voltage;
    /* LDO Module Status - 0xEC0A */
    boolean  ldo_vreg_ok;
    boolean  ldo_sc_detect;
    uint8    ldo_status;
    boolean  ldo_ireg_active;
    boolean  ldo_vgs_gt_2vt;
    /* NCP Module Status - 0xEC0B */
    boolean  ncp_vreg_ok;
    boolean  ncp_sc_detect;
    uint8    ncp_status;
} pm_lcdb_sts_type;

typedef enum
{
    PM_LCDB_ISENSE_TAP__X0P8,
    PM_LCDB_ISENSE_TAP__X0P9,
    PM_LCDB_ISENSE_TAP__X1,
    PM_LCDB_ISENSE_TAP__X1P1,
    PM_LCDB_ISENSE_TAP__INVALID
} pm_lcdb_isense_tap;

typedef enum
{
    PM_LCDB_ISENSE_GAIN__X0P5,
    PM_LCDB_ISENSE_GAIN__X1,
    PM_LCDB_ISENSE_GAIN__X1P5,
    PM_LCDB_ISENSE_GAIN__X2,
    PM_LCDB_ISENSE_GAIN__INVALID
} pm_lcdb_isense_gain;

typedef struct
{
    pm_lcdb_isense_tap  isense_tap;
    pm_lcdb_isense_gain  isense_gain;
} pm_lcdb_curr_sense_type;

typedef enum
{
    PM_LCDB_PS_THRESHOLD__50MA,
    PM_LCDB_PS_THRESHOLD__60MA,
    PM_LCDB_PS_THRESHOLD__70MA,
    PM_LCDB_PS_THRESHOLD__80MA,
    PM_LCDB_PS_THRESHOLD__INVALID
} pm_lcdb_ps_threshold;

typedef struct
{
    boolean  en_ps;
    boolean  sel_dig_ps;
    boolean  sel_ps_table;
    pm_lcdb_ps_threshold  ps_threshold;
} pm_lcdb_ps_ctl_type;

typedef enum
{
    PM_LCDB_CONFIG__BOOST_LDO_NCP,
    PM_LCDB_CONFIG__BOOST_LDO,
    PM_LCDB_CONFIG__BOOST_NCP,
    PM_LCDB_CONFIG__BOOST_ONLY,
    PM_LCDB_CONFIG__LDO_ONLY,
    PM_LCDB_CONFIG__NCP_ONLY,
    PM_LCDB_CONFIG__INVALID
} pm_lcdb_config_sel;

typedef enum
{
    PM_LCDB_PFM_HYSTERESIS__15MV,
    PM_LCDB_PFM_HYSTERESIS__25MV,
    PM_LCDB_PFM_HYSTERESIS__35MV,
    PM_LCDB_PFM_HYSTERESIS__45MV,
    PM_LCDB_PFM_HYSTERESIS__INVALID
} pm_lcdb_pfm_hysteresis;

typedef enum
{
    PM_LCDB_PFM_PEAK_CURRENT__300MA,
    PM_LCDB_PFM_PEAK_CURRENT__400MA,
    PM_LCDB_PFM_PEAK_CURRENT__500MA,
    PM_LCDB_PFM_PEAK_CURRENT__600MA,
    PM_LCDB_PFM_PEAK_CURRENT__INVALID
} pm_lcdb_pfm_peak_current;

typedef struct
{
    boolean  en_pfm;
    pm_lcdb_pfm_hysteresis  pfm_hysteresis;
    pm_lcdb_pfm_peak_current  pfm_peak_current;
    boolean  spare;
    boolean  byp_bst_soft_start_comp;
} pm_lcdb_pfm_ctl_type;

typedef enum
{
    PM_LCDB_NCP_ILIM_SS_SD_CTL__260MA,
    PM_LCDB_NCP_ILIM_SS_SD_CTL__460MA,
    PM_LCDB_NCP_ILIM_SS_SD_CTL__640MA,
    PM_LCDB_NCP_ILIM_SS_SD_CTL__810MA,
    PM_LCDB_NCP_ILIM_SS_SD_CTL__INVALID
} pm_lcdb_ncp_ilim_ss_sd_ctl;

typedef struct
{
    boolean  en_ncp_ilim;
    boolean  byp_ncpgd_to_ilim;
    boolean  en_ncp_ilim_gain;
    pm_lcdb_ncp_ilim_ss_sd_ctl  ncp_ilim_max_ss_sd;
} pm_lcdb_ncp_ilim_ctl_type;

typedef enum
{
    PM_LCDB_NCP_SOFTSTART_TIME__0MS,
    PM_LCDB_NCP_SOFTSTART_TIME__0P5MS,
    PM_LCDB_NCP_SOFTSTART_TIME__1MS,
    PM_LCDB_NCP_SOFTSTART_TIME__2MS,
    PM_LCDB_NCP_SOFTSTART_TIME__INVALID
} pm_lcdb_ncp_softstart_ctl;


/*===========================================================================

                        GLOBAL DATA DEFINITIONS

===========================================================================*/
static uint32 pm_lcdb_boost_op_voltage[] = /* array size 32 */
{
    4700, 4750, 4800, 4850, 4900, 4950, 5000, 5050, 5100, 5150,
    5200, 5250, 5300, 5350, 5400, 5450, 5500, 5550, 5600, 5650,
    5700, 5750, 5800, 5850, 5900, 5950, 6000, 6050, 6100, 6150,
    6200, 6250
};
static uint32 pm_lcdb_boost_op_voltage_size = sizeof(pm_lcdb_boost_op_voltage)/sizeof(pm_lcdb_boost_op_voltage[0]);

static uint32 pm_lcdb_ldo_op_voltage[] = /* array size 32 */
{
    4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900,
    4950, 5000, 5050, 5100, 5150, 5200, 5250, 5300, 5350, 5400,
    5450, 5500, 5550, 5600, 5650, 5700, 5750, 5800, 5850, 5900,
    5950, 6000
};
static uint32 pm_lcdb_ldo_op_voltage_size = sizeof(pm_lcdb_ldo_op_voltage)/sizeof(pm_lcdb_ldo_op_voltage[0]);

/* Below NCP Output Voltage values are (-ve) voltage values */
static uint32 pm_lcdb_ncp_op_voltage[] = /* array size 32 */
{
    4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900,
    4950, 5000, 5050, 5100, 5150, 5200, 5250, 5300, 5350, 5400,
    5450, 5500, 5550, 5600, 5650, 5700, 5750, 5800, 5850, 5900,
    5950, 6000
};
static uint32 pm_lcdb_ncp_op_voltage_size = sizeof(pm_lcdb_ncp_op_voltage)/sizeof(pm_lcdb_ncp_op_voltage[0]);


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
pm_err_flag_type pm_lcdb_get_status(uint8 pmic_index,
    pm_lcdb_sts_type *status
    );
pm_err_flag_type pm_lcdb_set_boost_output_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    );
pm_err_flag_type pm_lcdb_set_config_sel(uint8 pmic_index,
    pm_lcdb_config_sel config_sel
    );
pm_err_flag_type pm_lcdb_set_module_rdy(uint8 pmic_index,
    boolean module_rdy
    );
pm_err_flag_type pm_lcdb_module_enable(uint8 pmic_index,
    boolean module_en
    );
pm_err_flag_type pm_lcdb_module_hw_enable(uint8 pmic_index,
    boolean hwen_rdy
    );
pm_err_flag_type pm_lcdb_curr_sense_cfg(uint8 pmic_index,
    pm_lcdb_curr_sense_type *curr_sense
    );
pm_err_flag_type pm_lcdb_ps_ctl(uint8 pmic_index,
    pm_lcdb_ps_ctl_type *ps_ctl
    );
pm_err_flag_type pm_lcdb_pfm_ctl(uint8 pmic_index,
    pm_lcdb_pfm_ctl_type *pfm_ctl
    );
pm_err_flag_type pm_lcdb_set_ldo_output_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    );
pm_err_flag_type pm_lcdb_set_ncp_output_voltage(uint8 pmic_index,
    boolean en_ncp_vout,
    int32 op_voltage_mv /* negative value */
    );
pm_err_flag_type pm_lcdb_ncp_ilim_ctl(uint8 pmic_index,
    pm_lcdb_ncp_ilim_ctl_type *ncp_ilim_ctl
    );
pm_err_flag_type pm_lcdb_ncp_soft_start_ctl(uint8 pmic_index,
    pm_lcdb_ncp_softstart_ctl ncp_softstart_time
    );

#endif  /* PM_LCDB_H */
