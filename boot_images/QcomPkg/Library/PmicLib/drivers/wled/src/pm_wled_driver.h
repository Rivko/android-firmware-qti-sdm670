#ifndef __PM_WLED_DRIVER_H__
#define __PM_WLED_DRIVER_H__

/*! \file
 *  \n
 *  \brief  pm_wled_driver.h 
 *  \details  This file contains functions prototypes and variable/type/constant
 *  declarations for supporting WLED pin services for the Qualcomm
 *  PMIC chip set.
 *  \n &copy;  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ---     ----------------------------------------------------------
03/29/16    al      Updating for newer target
07/16/14    akm     Comm change Updates
03/05/14    aab     Updated to the latest PMIC driver architecture    
04/25/13    aab	    Adapted a reduced version of WLED driver for boot build   
========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/

#include "pm_target_information.h"


/*===========================================================================

                     WLED TYPES AND STRUCTURES 

===========================================================================*/
typedef struct
{
    pm_register_address_type base_address;                  // 0xD800

    pm_register_address_type revision2;                     // 0xD801
    pm_register_address_type revision3;                     // 0xD802
    pm_register_address_type revision4;                     // 0xD803
    pm_register_address_type perph_type;                    // 0xD804
    pm_register_address_type perph_subtype;                 // 0xD805
    pm_register_address_type fault_status;                  // 0xD808
    pm_register_address_type int_rt_sts;                    // 0xD810
    pm_register_address_type int_set_type;                  // 0xD811
    pm_register_address_type int_polarity_high;             // 0xD812
    pm_register_address_type int_polarity_low;              // 0xD813
    pm_register_address_type int_latched_clr;               // 0xD814
    pm_register_address_type int_en_set;                    // 0xD815
    pm_register_address_type int_en_clr;                    // 0xD816
    pm_register_address_type int_latched_sts;               // 0xD818
    pm_register_address_type int_pending_sts;               // 0xD819
    pm_register_address_type int_mid_sel;                   // 0xD81A
    pm_register_address_type int_priority;                  // 0xD81B
    pm_register_address_type en_amoled;                     // 0xD844
    pm_register_address_type module_enable;                 // 0xD846
    pm_register_address_type feedback_control;              // 0xD848
    pm_register_address_type hdr_adim_hdim;                 // 0xD849
    pm_register_address_type boost_min_on_off_ctl;          // 0xD84B
    pm_register_address_type switching_frequency;           // 0xD84C
    pm_register_address_type wled_ovp;                      // 0xD84D
    pm_register_address_type wled_ilim;                     // 0xD84E
    pm_register_address_type amoled_vout;                   // 0xD84F
    pm_register_address_type current_sense_ratio;           // 0xD850
    pm_register_address_type slope_comp_adj;                // 0xD851
    pm_register_address_type bias_gen_warmup_delay;         // 0xD852
    pm_register_address_type softstart_ramp_delay;          // 0xD853
    pm_register_address_type sw_slew_rate_control;          // 0xD854
    pm_register_address_type vloop_comp_gain;               // 0xD855
    pm_register_address_type vloop_comp_gm;                 // 0xD856
    pm_register_address_type vloop_comp_zero;               // 0xD857
    pm_register_address_type vloop_comp_hcap;               // 0xD858
    pm_register_address_type en_hw_bl_redn;                 // 0xD859
    pm_register_address_type en_psm;                        // 0xD85A 
    pm_register_address_type psm_ctrl;                      // 0xD85B
    pm_register_address_type wled_pfm;                      // 0xD85D
    pm_register_address_type wled_short_protect;            // 0xD85E
    pm_register_address_type swire_avdd_default;            // 0xD85F
    pm_register_address_type sec_access;                    // 0xD8D0
    pm_register_address_type test1;                         // 0xD8E2
    pm_register_address_type test3;                         // 0xD8E4
    pm_register_address_type test4;                         // 0xD8E5
}pm_wled_ctrl_register_info_type;


typedef struct 
{
    pm_register_address_type base_address;                   //0xD900

    pm_register_address_type   revision1;                    //0x1D900
    pm_register_address_type   revision2;                    //0x1D901
    pm_register_address_type   revision3;	             //0x1D902
    pm_register_address_type   revision4;                    //0x1D903
    pm_register_address_type   perph_type;                   //0x1D904
    pm_register_address_type   perph_subtype;                //0x1D905
    pm_register_address_type   current_sink_en;              //0x1D946
    pm_register_address_type   iled_sync_bit;                //0x1D947
    pm_register_address_type   dig_dim_clk_sel;              //0x1D948
    pm_register_address_type   modulation_scheme;            //0x1D94A
    pm_register_address_type   hybrid_dimming_tresh;         //0x1D94B
    pm_register_address_type   led1_modulator_en;            //0x1D950
    pm_register_address_type   led1_idac_sync_delay;         //0x1D951
    pm_register_address_type   led1_full_scale_current;      //0x1D952
    pm_register_address_type   led1_modulator_src_sel;       //0x1D953
    pm_register_address_type   led1_cabc_en;                 //0x1D956
    pm_register_address_type   led1_brightness_setting_lsb;  //0x1D957
    pm_register_address_type   led1_brightness_setting_msb;  //0x1D958
    pm_register_address_type   led2_modulator_en;            //0x1D960
    pm_register_address_type   led2_idac_sync_delay;         //0x1D961
    pm_register_address_type   led2_full_scale_current;	     //0x1D962
    pm_register_address_type   led2_modulator_src_sel;        //0x1D963
    pm_register_address_type   led2_cabc_en;                 //0x1D966
    pm_register_address_type   led2_brightness_setting_lsb;  //0x1D967
    pm_register_address_type   led2_brightness_setting_msb;  //0x1D968
    pm_register_address_type   led3_modulator_en;            //0x1D970
    pm_register_address_type   led3_idac_sync_delay;         //0x1D971
    pm_register_address_type   led3_full_scale_current;      //0x1D972
    pm_register_address_type   led3_modulator_src_sel;       //0x1D973
    pm_register_address_type   led3_cabc_en;                 //0x1D976
    pm_register_address_type   led3_brightness_setting_lsb;  //0x1D977
    pm_register_address_type   led3_brightness_setting_msb;  //0x1D978
    pm_register_address_type   led4_modulator_en;            //0x1D980
    pm_register_address_type   led4_idac_sync_delay;         //0x1D981
    pm_register_address_type   led4_full_scale_current;      //0x1D982
    pm_register_address_type   led4_modulator_src_sel;       //0x1D983
    pm_register_address_type   led4_cabc_en;                 //0x1D986
    pm_register_address_type   led4_brightness_setting_lsb;  //0x1D987
    pm_register_address_type   led4_brightness_setting_msb;  //0x1D988
    pm_register_address_type   sec_access;                   //0x1D9D0
}pm_wled_sink_register_info_type;


typedef struct
{
    pm_wled_ctrl_register_info_type*   wled_ctrl_reg;
    pm_wled_sink_register_info_type*   wled_sink_reg;
}pm_wled_register_info_type;


typedef struct
{
	pm_comm_info_type           *comm_ptr;
    pm_wled_register_info_type  *wled_reg_table;
    uint8             num_of_peripherals;
}pm_wled_data_type;
/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_wled_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_wled_data_type* pm_wled_get_data(uint8 pmic_index);

uint8 pm_wled_get_num_peripherals(uint8 pmic_index);

#endif //__PM_WLED_DRIVER_H__
