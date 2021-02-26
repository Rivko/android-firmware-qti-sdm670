#ifndef PM_OLEDB_H
#define PM_OLEDB_H

/*! \file  pm_oledb.h
 *  \n
 *  \brief  PMIC-OLEDB MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC OLEDB module.
 *
 *  \n &copy; Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated. All Rights Reserved.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ----    ---------------------------------------------------------
04/11/17    ks      Added oledb swire spare bit get and clear api's(CR-2004206)
02/07/17    sv      Added pm_oledb_pd_ctrl API
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

#define PM_OLEDB_SOFTSTART_IOUT_OFFSET  0x07  /* SOFTSTART_RAMP_DELAY - 0xE053 bits<5:3> */
#define PM_OLEDB_SOFTSTART_ILIM_TIME    0x07  /* SOFTSTART_RAMP_DELAY - 0xE053 bits<2:0>, RampTime = 40us * (sstart_ramp_t + 1) */


typedef struct
{
   boolean  sc_fault;
   boolean  ol_fault;
   boolean  ilim_fault;
} pm_oledb_sts_type;

typedef enum
{
    PM_OLEDB_SWIRE_CONTROL__PD_EN,
    PM_OLEDB_SWIRE_CONTROL__VOUT_EN,
    PM_OLEDB_SWIRE_CONTROL__INVALID
} pm_oledb_swire_control;

typedef enum
{
    PM_OLEDB_VOUT_PROGRAM__VOUT_REQ,
    PM_OLEDB_VOUT_PROGRAM__VOUT_DEFAULT,
    PM_OLEDB_VOUT_PROGRAM__INVALID
} pm_oledb_vout_program;

typedef enum
{
    PM_OLEDB_SOFTSTART_TIME_VREF__1400uS,
    PM_OLEDB_SOFTSTART_TIME_VREF__875uS,
    PM_OLEDB_SOFTSTART_TIME_VREF__700uS,
    PM_OLEDB_SOFTSTART_TIME_VREF__525uS,
    PM_OLEDB_SOFTSTART_TIME_VREF__INVALID
} pm_oledb_softstart_time_vref;

typedef struct
{
   pm_oledb_softstart_time_vref  ss_time_vref;
   uint8  ss_iout_offset;
   uint8  ss_ilim_time;
} pm_oledb_softstart_ctl_type;

typedef enum
{
    PM_OLEDB_PSM_VREF__440mV,
    PM_OLEDB_PSM_VREF__510mV,
    PM_OLEDB_PSM_VREF__580mV,
    PM_OLEDB_PSM_VREF__650mV,
    PM_OLEDB_PSM_VREF__715mV,
    PM_OLEDB_PSM_VREF__780mV,
    PM_OLEDB_PSM_VREF__850mV,
    PM_OLEDB_PSM_VREF__920mV,
    PM_OLEDB_PSM_VREF__INVALID
} pm_oledb_psm_vref;

typedef struct
{
   boolean  en_psm;
   boolean  psm_hys_ctrl;
   pm_oledb_psm_vref  psm_vref;
} pm_oledb_psm_ctl_type;

typedef enum
{
    PM_OLEDB_PFM_ILIM__130mA,
    PM_OLEDB_PFM_ILIM__200mA,
    PM_OLEDB_PFM_ILIM__270mA,
    PM_OLEDB_PFM_ILIM__340mA,
    PM_OLEDB_PFM_ILIM__INVALID
} pm_oledb_pfm_ilim;

/* NFET off time at PFM or at ILIM event of PWM */
typedef enum
{
    PM_OLEDB_PFM_TOFF__110nS_25nS,
    PM_OLEDB_PFM_TOFF__240nS_45nS,
    PM_OLEDB_PFM_TOFF__350nS_70nS,
    PM_OLEDB_PFM_TOFF__480nS_90nS,
    PM_OLEDB_PFM_TOFF__INVALID
} pm_oledb_pfm_toff;

typedef struct
{
   boolean  en_psm;
   boolean  pfm_hys_ctrl;
   pm_oledb_pfm_ilim  pfm_ilim;
   pm_oledb_pfm_toff  pfm_toff;
} pm_oledb_pfm_ctl_type;


/*===========================================================================

                        GLOBAL DATA DEFINITIONS

===========================================================================*/
static uint32 pm_oledb_op_voltage[] = /* array size 32 */
{
    5000, 5100, 5200, 5300, 5400, 5500, 5600, 5700, 5800, 5900,
    6000, 6100, 6200, 6300, 6400, 6500, 6600, 6700, 6800, 6900,
    7000, 7100, 7200, 7300, 7400, 7500, 7600, 7700, 7800, 7900,
    8000, 8100
};
static uint32 pm_oledb_op_voltage_size = sizeof(pm_oledb_op_voltage)/sizeof(pm_oledb_op_voltage[0]);


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
pm_err_flag_type pm_oledb_get_status(uint8 pmic_index,
    pm_oledb_sts_type *status
    );
pm_err_flag_type pm_oledb_set_module_rdy(uint8 pmic_index,
    boolean module_rdy
    );
pm_err_flag_type pm_oledb_module_enable(uint8 pmic_index,
    boolean en_module
    );
pm_err_flag_type pm_oledb_set_ext_pin_ctl(uint8 pmic_index,
    boolean ext_pin_ctl
    );
pm_err_flag_type pm_oledb_swire_ctl(uint8 pmic_index,
    pm_oledb_swire_control swire_control,
    boolean enable
    );
pm_err_flag_type pm_oledb_vout_programming(uint8 pmic_index,
    pm_oledb_vout_program vout_program,
    uint32 op_voltage_mv
    );
pm_err_flag_type pm_oledb_pd_ctrl(uint8 pmic_index,
    boolean pd_enable
    );
pm_err_flag_type pm_oledb_softstart_ctl(uint8 pmic_index,
    pm_oledb_softstart_ctl_type *softstart_ctl
    );
pm_err_flag_type pm_oledb_psm_ctl(uint8 pmic_index,
    pm_oledb_psm_ctl_type *psm_ctl
    );
pm_err_flag_type pm_oledb_pfm_ctl(uint8 pmic_index,
    pm_oledb_pfm_ctl_type *pfm_ctl
    );
pm_err_flag_type pm_oledb_get_swire_sparebit_status(uint8 pmic_index,
    boolean *SparebitEnabled
    );
pm_err_flag_type pm_oledb_clear_swire_sparebit_status(uint8 pmic_index
    );

#endif  /* PM_OLEDB_H */
