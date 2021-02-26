#ifndef __PM_FG_DRIVER_H__
#define __PM_FG_DRIVER_H__

/*! \file
*  \n
*  \brief  pm_fg_driver.h 
*  \details  
*  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/31/17    va     Adding DMA arb reg
12/19/16    sm     Added API to get FG PmicIndex
01/27/16    al     New File
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"

/*===========================================================================

                        TYPE DEFINES AND ENUMS

===========================================================================*/
/*  fg_batt_soc_register_ds */
typedef struct
{
  pm_register_address_type base_address;           //0x4000
  pm_register_address_type perph_type;             //0x004
  pm_register_address_type perph_subtype;          //0x005
  pm_register_address_type fg_alg_sts;             //0x006
  pm_register_address_type fg_alg_aux_sts0;        //0x007
  pm_register_address_type sleep_shutdown_sts;     //0x008
  pm_register_address_type monotonic_soc;       //0x009
  pm_register_address_type monotonic_soc_cp;    //0x00A
  pm_register_address_type int_rt_sts;             //0x010
  pm_register_address_type int_set_type;           //0x011
  pm_register_address_type int_polarity_high;      //0x012
  pm_register_address_type int_polarity_low;       //0x013
  pm_register_address_type int_latched_clr;        //0x014
  pm_register_address_type int_en_set;             //0x015
  pm_register_address_type int_en_clr;             //0x016
  pm_register_address_type int_latched_sts;        //0x018
  pm_register_address_type int_pending_sts;        //0x019
  pm_register_address_type int_mid_sel;            //0x01A
  pm_register_address_type int_priority;           //0x01B
  pm_register_address_type en_ctl;                 //0x046
  pm_register_address_type restart;                //0x048
  pm_register_address_type sts_clr;                //0x04A
  pm_register_address_type low_pwr_cfg;            //0x052
  pm_register_address_type low_pwr_sts;            //0x056
  pm_register_address_type debug_sts;              //0x0B6
  pm_register_address_type fg_cya_cfg;             //0x0B8
  pm_register_address_type rst_ctrl;               //0x0BA
  pm_register_address_type sec_access;             //0x0D0
}fg_batt_soc_register_ds;


/*fg_batt_info_register_ds*/
typedef struct
{
  pm_register_address_type base_address;              //0x4100
  pm_register_address_type perph_type;               // 0x004
  pm_register_address_type perph_subtype;            // 0x005
  pm_register_address_type battery_temp_sts;         // 0x006
  pm_register_address_type sys_batt;                 // 0x007
  pm_register_address_type fg_sts;                   // 0x009
  pm_register_address_type int_rt_sts;               // 0x010
  pm_register_address_type int_set_type;             // 0x011
  pm_register_address_type int_polarity_high;        // 0x012
  pm_register_address_type int_polarity_low;         // 0x013
  pm_register_address_type int_latched_clr;          // 0x014
  pm_register_address_type int_en_set;               // 0x015
  pm_register_address_type int_en_clr;               // 0x016
  pm_register_address_type int_latched_sts;          // 0x018
  pm_register_address_type int_pending_sts;          // 0x019
  pm_register_address_type int_mid_sel;              // 0x01A
  pm_register_address_type int_priority;             // 0x01B
  pm_register_address_type smart_battery;            // 0x048
  pm_register_address_type batt_removed_latched;     // 0x04F
  pm_register_address_type battery_temperature_lsb;  // 0x050
  pm_register_address_type battery_temperature_msb;  // 0x051
  pm_register_address_type battery_temp_cfg;         // 0x056
  pm_register_address_type batt_tmpr_intr;           // 0x059
  pm_register_address_type therm_therm_c1;           // 0x05C
  pm_register_address_type therm_therm_c2;           // 0x05D
  pm_register_address_type therm_therm_c3;           // 0x05E
  pm_register_address_type therm_therm_half_range;   // 0x05F
  pm_register_address_type jeita_ctls;               // 0x061
  pm_register_address_type jeita_too_cold;           // 0x062
  pm_register_address_type jeita_cold;               // 0x063
  pm_register_address_type jeita_hot;                // 0x064
  pm_register_address_type jeita_too_hot;            // 0x065
  pm_register_address_type esr_cfg;                  // 0x069
  pm_register_address_type battery_missing_cfg;      // 0x06B
  pm_register_address_type watchdog_count;           // 0x070
  pm_register_address_type watchdog_cfg;             // 0x071
  pm_register_address_type ibatt_sensing_cfg;        // 0x073
  pm_register_address_type qnovo_cfg;                // 0x074
  pm_register_address_type qnovo_scaler;             // 0x075
  pm_register_address_type vbatt_msb;                // 0x0A0
  pm_register_address_type vbatt_lsb;                // 0x0A1
  pm_register_address_type ibatt_msb;                // 0x0A2
  pm_register_address_type ibatt_lsb;                // 0x0A3
  pm_register_address_type esr_msb;                  // 0x0A4
  pm_register_address_type esr_lsb;                  // 0x0A5
  pm_register_address_type vbatt_msb_cp;             // 0x0A6
  pm_register_address_type vbatt_lsb_cp;             // 0x0A7
  pm_register_address_type ibatt_msb_cp;             // 0x0A8
  pm_register_address_type ibatt_lsb_cp;             // 0x0A9
  pm_register_address_type esr_msb_cp;               // 0x0AA
  pm_register_address_type esr_lsb_cp;               // 0x0AB
  pm_register_address_type batt_vadc_msb;            // 0x0AC
  pm_register_address_type batt_vadc_lsb;            // 0x0AD
  pm_register_address_type batt_iadc_msb;            // 0x0AE
  pm_register_address_type batt_iadc_lsb;            // 0x0AF
  pm_register_address_type sec_access;               // 0x0D0
}fg_batt_info_register_ds;

/*fg_bcl_register_ds*/
typedef struct
{
  pm_register_address_type base_address;             //0x4200
  pm_register_address_type perph_type;               //0x004
  pm_register_address_type perph_subtype;            //0x005
  pm_register_address_type bcl_monitor_sts;          //0x006
  pm_register_address_type batt_vi_monitor;          //0x007
  pm_register_address_type lmh_sts;                  //0x008
  pm_register_address_type ban_batfet_open_sts;      //0x009
  pm_register_address_type int_rt_sts;               //0x010
  pm_register_address_type int_set_type;             //0x011
  pm_register_address_type int_polarity_high;        //0x012
  pm_register_address_type int_polarity_low;         //0x013
  pm_register_address_type int_latched_clr;          //0x014
  pm_register_address_type int_en_set;               //0x015
  pm_register_address_type int_en_clr;               //0x016
  pm_register_address_type int_latched_sts;          //0x018
  pm_register_address_type int_pending_sts;          //0x019
  pm_register_address_type int_mid_sel;              //0x01A
  pm_register_address_type int_priority;             //0x01B
  pm_register_address_type en_ctl;                   //0x046
  pm_register_address_type clear_ctls;               //0x048
  pm_register_address_type ibatt_dgl_ctls;           //0x04E
  pm_register_address_type vbatt_adc_dx_dgl_ctls;    //0x050
  pm_register_address_type vbatt_irq_dx_dgl_ctls;    //0x051
  pm_register_address_type vbatt_cmp_ax_dgl_ctls;    //0x052
  pm_register_address_type vbatt_cmp_dx_dgl_ctls;    //0x053
  pm_register_address_type ban_cfg;                  //0x054
  pm_register_address_type ban_ax_dgl_ctls;          //0x055
  pm_register_address_type ban_btftop_dgl_ctls;      //0x056
  pm_register_address_type bcl_sleep_dgl_ctls;       //0x057
  pm_register_address_type vbatt;                    //0x058
  pm_register_address_type ibatt;                    //0x059
  pm_register_address_type vbat_cp;                  //0x05A
  pm_register_address_type ibat_cp;                  //0x05B
  pm_register_address_type vbat_min;                 //0x05C
  pm_register_address_type ibat_max;                 //0x05D
  pm_register_address_type vbat_min_cp;              //0x05E
  pm_register_address_type ibat_max_cp;              //0x05F
  pm_register_address_type ibat_partial_0;           //0x060
  pm_register_address_type ibat_partial_1;           //0x061
  pm_register_address_type ibat_partial_0_cp;        //0x062
  pm_register_address_type ibat_partial_1_cp;        //0x063
  pm_register_address_type vbat_partial_0;           //0x064
  pm_register_address_type vbat_partial_1;           //0x065
  pm_register_address_type vbat_partial_0_cp;        //0x066
  pm_register_address_type vbat_partial_1_cp;        //0x067
  pm_register_address_type bcl_cfg;                  //0x06A
  pm_register_address_type vbat_cmp_cfg;             //0x070
  pm_register_address_type vbat_adc_low;             //0x072
  pm_register_address_type vbat_low;                 //0x075
  pm_register_address_type vbat_too_low;             //0x076
  pm_register_address_type ibatt_high;               //0x078
  pm_register_address_type ibatt_too_high;           //0x079
  pm_register_address_type ibatt_h_cfg;              //0x080
  pm_register_address_type ibatt_th_cfg;             //0x081
  pm_register_address_type vbatt_adc_cfg;            //0x084
  pm_register_address_type vbatt_l_cfg;              //0x086
  pm_register_address_type vbatt_l_int_cfg;          //0x087
  pm_register_address_type vbatt_tl_cfg;             //0x088
  pm_register_address_type vbatt_tl_int_cfg;         //0x089
  pm_register_address_type sleep_cfg;                //0x090
  pm_register_address_type ban_timing_ax_dgl;        //0x091
  pm_register_address_type ban_timing_dx;            //0x092
  pm_register_address_type ocp_btftoprv_timing;      //0x093
  pm_register_address_type lmh_fsm;                  //0x0A0
  pm_register_address_type lmh_from_bob;             //0x0A1
  pm_register_address_type lmh_from_flash;           //0x0A2
  pm_register_address_type lmh_cfg;                  //0x0A3
  pm_register_address_type lmh_delay;                //0x0A4
  pm_register_address_type lmh_pbs_trigger;          //0x0A5
  pm_register_address_type lmh_wil;                  //0x0A6
  pm_register_address_type lmh_bob_dgl;              //0x0A7
  pm_register_address_type lmh_bob_dgl_ctl;          //0x0A8
  pm_register_address_type trepn_sts;                //0x0B0
  pm_register_address_type trepn_cfg;                //0x0B2
  pm_register_address_type trepn_rd_cfg;             //0x0B3
  pm_register_address_type trepn_rd_ctl;             //0x0B4
  pm_register_address_type trepn_ibatt_lsb;          //0x0B6
  pm_register_address_type trepn_ibatt_msb;          //0x0B7
  pm_register_address_type trepn_ib_live_lsb;        //0x0B8
  pm_register_address_type trepn_ib_live_msb;        //0x0B9
  pm_register_address_type trepn_sample_count;       //0x0BA
  pm_register_address_type sec_access;               //0x0D0
}fg_bcl_register_ds;


/*fg_mem_if_register_ds*/
typedef struct
{
  pm_register_address_type base_address;            // 0x4400
  pm_register_address_type dig_major;               // 0x001
  pm_register_address_type perph_type;              // 0x004
  pm_register_address_type perph_subtype;           // 0x005
  pm_register_address_type ima_rdy_sts;             // 0x007
  pm_register_address_type int_rt_sts;              // 0x010
  pm_register_address_type int_set_type;            // 0x011
  pm_register_address_type int_polarity_high;       // 0x012
  pm_register_address_type int_polarity_low;        // 0x013
  pm_register_address_type int_latched_clr;         // 0x014
  pm_register_address_type int_en_set;              // 0x015
  pm_register_address_type int_en_clr;              // 0x016
  pm_register_address_type int_latched_sts;         // 0x018
  pm_register_address_type int_pending_sts;         // 0x019
  pm_register_address_type int_mid_sel;             // 0x01A
  pm_register_address_type int_priority;            // 0x01B
  pm_register_address_type mem_arb_cfg;             // 0x040
  pm_register_address_type mem_intf_cfg;            // 0x050
  pm_register_address_type mem_intf_ctl;            // 0x051
  pm_register_address_type ima_cfg;                 // 0x052
  pm_register_address_type ima_operation_sts;       // 0x054
  pm_register_address_type ima_exception_sts;       // 0x055
  pm_register_address_type ima_hardware_sts;        // 0x056
  pm_register_address_type fg_beat_count;           // 0x057
  pm_register_address_type ima_err_sts;             // 0x05F
  pm_register_address_type ima_byte_en;             // 0x060
  pm_register_address_type mem_intf_addr_lsb;       // 0x061
  pm_register_address_type mem_intf_addr_msb;       // 0x062
  pm_register_address_type mem_intf_wr_data0;       // 0x063
  pm_register_address_type mem_intf_wr_data1;       // 0x064
  pm_register_address_type mem_intf_wr_data2;       // 0x065
  pm_register_address_type mem_intf_wr_data3;       // 0x066
  pm_register_address_type mem_intf_rd_data0;       // 0x067
  pm_register_address_type mem_intf_rd_data1;       // 0x068
  pm_register_address_type mem_intf_rd_data2;       // 0x069
  pm_register_address_type mem_intf_rd_data3;       // 0x06A
  pm_register_address_type dma_sts;                 // 0x070
  pm_register_address_type dma_intf_ctl;            // 0x071
  pm_register_address_type dma_cfg;                 // 0x072
  pm_register_address_type sec_access;              // 0x0D0
}fg_mem_if_register_ds;

/*fg_adc_rr_register_ds*/
typedef struct
{
  pm_register_address_type base_address;           //0x4500
  pm_register_address_type perph_type;             //0x004
  pm_register_address_type perph_subtype;          //0x005
  pm_register_address_type status1;                //0x008
  pm_register_address_type chg_pwr_sts;            //0x009
  pm_register_address_type ref_sts;                //0x00A
  pm_register_address_type int_rt_sts;             //0x010
  pm_register_address_type int_set_type;           //0x011
  pm_register_address_type int_polarity_high;      //0x012
  pm_register_address_type int_polarity_low;       //0x013
  pm_register_address_type int_latched_clr;        //0x014
  pm_register_address_type int_en_set;             //0x015
  pm_register_address_type int_en_clr;             //0x016
  pm_register_address_type int_latched_sts;        //0x018
  pm_register_address_type int_pending_sts;        //0x019
  pm_register_address_type int_mid_sel;            //0x01A
  pm_register_address_type int_priority;           //0x01B
  pm_register_address_type en_ctl;                 //0x046
  pm_register_address_type skin_temperature_lsb;   //0x050
  pm_register_address_type skin_temperature_msb;   //0x051
  pm_register_address_type rr_adc_ctl;             //0x052
  pm_register_address_type rr_adc_log;             //0x053
  pm_register_address_type ref_cfg;                //0x054
  pm_register_address_type rbias_cnls;             //0x055
  pm_register_address_type fake_batt_low_lsb;      //0x058
  pm_register_address_type fake_batt_low_msb;      //0x059
  pm_register_address_type fake_batt_high_lsb;     //0x05A
  pm_register_address_type fake_batt_high_msb;     //0x05B
  pm_register_address_type batt_id_ctrl;           //0x060
  pm_register_address_type batt_id_trigger;        //0x061
  pm_register_address_type batt_id_sts;            //0x062
  pm_register_address_type batt_id_cfg;            //0x063
  pm_register_address_type batt_id_5_lsb;          //0x066
  pm_register_address_type batt_id_5_msb;          //0x067
  pm_register_address_type batt_id_15_lsb;         //0x068
  pm_register_address_type batt_id_15_msb;         //0x069
  pm_register_address_type batt_id_150_lsb;        //0x06A
  pm_register_address_type batt_id_150_msb;        //0x06B
  pm_register_address_type batt_therm_ctrl;        //0x070
  pm_register_address_type batt_therm_trigger;     //0x071
  pm_register_address_type batt_therm_sts;         //0x072
  pm_register_address_type batt_therm_cfg;         //0x073
  pm_register_address_type batt_therm_lsb;         //0x074
  pm_register_address_type batt_therm_msb;         //0x075
  pm_register_address_type batt_therm_freq;        //0x076
  pm_register_address_type aux_therm_ctrl;         //0x080
  pm_register_address_type aux_therm_trigger;      //0x081
  pm_register_address_type aux_therm_sts;          //0x082
  pm_register_address_type aux_therm_cfg;          //0x083
  pm_register_address_type aux_therm_lsb;          //0x084
  pm_register_address_type aux_therm_msb;          //0x085
  pm_register_address_type skin_hot;               //0x086
  pm_register_address_type skin_too_hot;           //0x087
  pm_register_address_type aux_therm_c1;           //0x088
  pm_register_address_type aux_therm_c2;           //0x089
  pm_register_address_type aux_therm_c3;           //0x08A
  pm_register_address_type aux_therm_half_range;   //0x08B
  pm_register_address_type usb_in_v_ctrl;          //0x090
  pm_register_address_type usb_in_v_trigger;       //0x091
  pm_register_address_type usb_in_v_sts;           //0x092
  pm_register_address_type usb_in_v_lsb;           //0x094
  pm_register_address_type usb_in_v_msb;           //0x095
  pm_register_address_type usb_in_i_ctrl;          //0x098
  pm_register_address_type usb_in_i_trigger;       //0x099
  pm_register_address_type usb_in_i_sts;           //0x09A
  pm_register_address_type usb_in_i_lsb;           //0x09C
  pm_register_address_type usb_in_i_msb;           //0x09D
  pm_register_address_type dc_in_v_ctrl;           //0x0A0
  pm_register_address_type dc_in_v_trigger;        //0x0A1
  pm_register_address_type dc_in_v_sts;            //0x0A2
  pm_register_address_type dc_in_v_lsb;            //0x0A4
  pm_register_address_type dc_in_v_msb;            //0x0A5
  pm_register_address_type dc_in_i_ctrl;           //0x0A8
  pm_register_address_type dc_in_i_trigger;        //0x0A9
  pm_register_address_type dc_in_i_sts;            //0x0AA
  pm_register_address_type dc_in_i_lsb;            //0x0AC
  pm_register_address_type dc_in_i_msb;            //0x0AD
  pm_register_address_type pmi_die_temp_ctrl;      //0x0B0
  pm_register_address_type pmi_die_temp_trigger;   //0x0B1
  pm_register_address_type pmi_die_temp_sts;       //0x0B2
  pm_register_address_type pmi_die_temp_cfg;       //0x0B3
  pm_register_address_type pmi_die_temp_lsb;       //0x0B4
  pm_register_address_type pmi_die_temp_msb;       //0x0B5
  pm_register_address_type charger_temp_ctrl;      //0x0B8
  pm_register_address_type charger_temp_trigger;   //0x0B9
  pm_register_address_type charger_temp_sts;       //0x0BA
  pm_register_address_type charger_temp_cfg;       //0x0BB
  pm_register_address_type charger_temp_lsb;       //0x0BC
  pm_register_address_type charger_temp_msb;       //0x0BD
  pm_register_address_type charger_hot;            //0x0BE
  pm_register_address_type charger_too_hot;        //0x0BF
  pm_register_address_type gpio_ctrl;              //0x0C0
  pm_register_address_type gpio_trigger;           //0x0C1
  pm_register_address_type gpio_sts;               //0x0C2
  pm_register_address_type gpio_lsb;               //0x0C4
  pm_register_address_type gpio_msb;               //0x0C5
  pm_register_address_type sec_access;             //0x0D0
}fg_adc_rr_register_ds;


typedef struct
{
  fg_batt_soc_register_ds   *batt_soc_register;
  fg_batt_info_register_ds  *batt_info_register;
  fg_bcl_register_ds        *bcl_register;
  fg_mem_if_register_ds     *mem_if_register;
  fg_adc_rr_register_ds     *adc_rr_register;
}fg_register_ds;

typedef struct
{
  pm_comm_info_type     *comm_ptr;
  fg_register_ds        *fg_register;
  uint8                 num_of_peripherals;
}pm_fg_data_type;


/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/

/* driver init */
void pm_fg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_fg_data_type* pm_fg_get_data(uint8 pmic_index);

uint8 pm_fg_get_num_peripherals(uint8 pmic_index);

void pm_fg_get_pmic_info(uint8 *pmic_index, uint8 *slave_id);

#endif // __PM_FG_DRIVER_H__
