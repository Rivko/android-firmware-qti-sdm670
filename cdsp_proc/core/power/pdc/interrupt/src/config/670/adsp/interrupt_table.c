/*==============================================================================
  FILE:         interrupt_table.c

  OVERVIEW:     PDC hardware to local interrupt mapping info.
 
  DEPENDENCIES: None
 
  Note:         Based on: napaliq_top.irq.flatV16.0

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/config/670/adsp/interrupt_table.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"

/*============================================================================
 *                              GLOBAL VARIABLES
 *===========================================================================*/
/* Fixed interrupts in the PDC */
pdc_interrupt_mapping g_pdcInterruptMapping[] = 
{
  /* Default trigger type     Default DRV         Subsystem Interrupt # */
  /* Bit 0 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     49},  /* pmic_arb_trans_done_irq[5] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     15},  /* ee5_ssc_spmi_periph_irq */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},      7},  /* rpm_ipc[24] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     30},  /* rpm_ipc[25] */
  
  /* Bit 4 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    238},  /* aoss_cti_trigout_4 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    239},  /* wind_down_req */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     10},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     18},  /* not-connected */
  
  /* Bit 8 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    240},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    241},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    242},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    114},  /* not-connected */
  
  /* Bit 12 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     35},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     39},  /* not-connected */ 
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     57},  /* eud_int_mx[6] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     42},  /* not-connected */
  
  /* Bit 16 */ 
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     43},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     44},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     36},  /* vfr_0 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     40},  /* vfr_1 */
  
  /* Bit 20 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     38},  /* summary_irq_lpa_dsp*/
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     45},  /* aop_ipc[6] */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},      9},  /* mss_ipc_out_irq[24] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},      6},  /* mss_ipc_out_irq[25] */
  
  /* Bit 24 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     22},  /* mss_ipc_out_irq[26] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     34},  /* usb30_power_event_irq */   
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     46},  /* APSS_sscHlosIPCInterrupt[0] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     33},  /* APSS_sscHlosIPCInterrupt[1] */
  
  /* Bit 28 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},      8},  /* APSS_sscHlosIPCInterrupt[2] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    120},  /* not-connected */    
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    102},  /* mdp_vsync_p */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    153},  /* mdp_vsync_s */
  
  /* Bit 32 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     48},  /* APSS_adspIPCInterrupt[0] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     63},  /* APSS_adspIPCInterrupt[1] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    110},  /* q6ss_irq_out[24] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    111},  /* q6ss_irq_out[25] */
  
  /* Bit 36 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     95},  /* q6ss_irq_out[26] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     78},  /* o_timeout_slave_lpass_summary_intr */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    243},  /* nav_gpio_dr_sync */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     90},  /* mss_ipc_out_irq[8] */
  
  /* Bit 40 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     62},  /* summary_irq_sensors */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     70},  /* dir_conn_irq_sensors_0 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     71},  /* dir_conn_irq_sensors_1 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     93},  /* top_qupv3_ssc_irq_out[0] */

  /* Bit 44 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     94},  /* top_qupv3_ssc_irq_out[1] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     64},  /* top_qupv3_lpass_irq_out[0] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     65},  /* top_qupv3_lpass_irq_out[1] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     89},  /* not-connected */
  
  /* bIT 48 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     47},  /* usb30_ctrl_irq[2] */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    246},  /* not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},     91},  /* mss_ipc_out_irq[9] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    100},  /* aop_ipc[7] */
 
  /* bIT 52 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    106},  /* q6ss_irq_out[8] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    107},  /* q6ss_irq_out[9] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    155},  /* mdp_vsync_e */
};

/* Size of the table */
const uint32 g_pdcInterruptTableSize = sizeof(g_pdcInterruptMapping) / sizeof(g_pdcInterruptMapping[0]);
