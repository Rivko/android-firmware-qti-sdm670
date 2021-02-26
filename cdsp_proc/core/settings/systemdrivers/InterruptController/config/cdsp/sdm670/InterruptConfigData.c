
/*==============================================================================

  D A L I N T E R R U P T   C O N T R O L L E R

  DESCRIPTION
    This auto-generated file contains the interrupt configuration data for
    all the interrupts for this platform.

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/InterruptController/config/cdsp/sdm670/InterruptConfigData.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
===========================================================================*/

#include "DALReg.h"
#include "DDIInterruptController.h"
#include "InterruptControllerInterface.h"

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * InterruptConfigs
 * BSP data for this target's interrupts.
 * The data layout is the following:
 * { L2VIC interrupt vector number, interrupt name, trigger type}
 *
 */
static InterruptConfigType InterruptConfigs[] =
{
  {0, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {1, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {2, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {3, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {4, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {5, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {7, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "vapss_hcp_noc_err_irq"},
  {9, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "vapss_dma_noc_err_irq"},
  {11, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "vapss_tcm_noc_err_irq"},
  {14, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[0]"},
  {16, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {21, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[1]"},
  {28, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {29, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[2]"},
  {31, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {32, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "mss_ipc_out_irq[13]"},
  {33, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "aop_ipc[18]"},
  {34, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "APSS_cdspIPCInterrupt[2]"},
  {35, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "mss_ipc_out_irq[12]"},
  {36, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[0]"},
  {37, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "turing_noc_err_irq"},
  {41, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {42, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[6]"},
  {44, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "APSS_cdspIPCInterrupt[3]"},
  {45, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[1]"},
  {46, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {47, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[3]"},
  {48, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "mss_ipc_out_irq[14]"},
  {50, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "dir_conn_irq_turing[0]"},
  {51, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "dir_conn_irq_turing[1]"},
  {52, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[7]"},
  {53, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[2]"},
  {55, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[4]"},
  {56, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "aop_ipc[19]"},
  {57, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[8]"},
  {58, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {59, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "APSS_cdspIPCInterrupt[1]"},
  {60, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "mss_ipc_out_irq[15]"},
  {61, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {63, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[3]"},
  {64, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "summary_irq_turing"},
  {67, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "usb30_ctrl_irq[2]"},
  {68, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[9]"},
  {69, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "o_cdsp_bwmon_intr"},
  {70, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "usb30_power_event_irq"},
  {71, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[5]"},
  {72, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "APSS_cdspIPCInterrupt[0]"},
  {74, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[19]"},
  {75, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[4]"},
  {77, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[10]"},
  {79, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[6]"},
  {80, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[5]"},
  {82, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[11]"},
  {83, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "mdp_vsync_e"},
  {85, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_gsi_irq[12]"},
  {86, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[7]"},
  {87, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "spdm_realtime_irq[2]"},
  {89, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[0]"},
  {90, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[0]"},
  {91, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[1]"},
  {92, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[2]"},
  {93, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[3]"},
  {94, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[4]"},
  {95, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "top_qupv3_turing_irq_out[5]"},
  {96, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[1]"},
  {105, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "ssc_irq_out_compute[0]"},
  {106, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "ssc_qupv3_turing_irq_out[0]"},
  {107, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "ssc_qupv3_turing_irq_out[1]"},
  {109, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[8]"},
  {110, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[2]"},
  {112, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[9]"},
  {113, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[3]"},
  {116, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "o_timeout_slave_turing_summary_intr"},
  {117, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[10]"},
  {118, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[4]"},
  {119, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "ssc_irq_out_compute[3]"},
  {121, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "lpass_irq_out_cdsp[2]"},
  {122, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "crypto_core_irq[2]"},
  {123, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "crypto_bam_irq[2]"},
  {127, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[11]"},
  {128, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "qupv3_sensor_se_irq[5]"},
  {134, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "processor_0_user_int"},
  {135, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "processor_0_kernel_int"},
  {136, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "lpass_irq_out_cdsp[0]"},
  {137, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "lpass_irq_out_cdsp[1]"},
  {138, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[12]"},
  {139, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {144, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "ssc_irq_out_compute[1]"},
  {145, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "ssc_irq_out_compute[2]"},
  {152, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[13]"},
  {158, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[14]"},
  {163, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[15]"},
  {168, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[16]"},
  {169, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "mdp_vsync_p"},
  {170, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {171, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {175, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[17]"},
  {178, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {179, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "mdp_vsync_s"},
  {181, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "compute_pdc_irq_out[18]"},
  {190, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, "na"},
  {201, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_bps_cdm_irq"},
  {202, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_bps_irq"},
  {203, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_camnoc_irq"},
  {204, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_cci_irq"},
  {205, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_cdm_irq"},
  {206, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_fd_irq"},
  {207, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_icp_irq"},
  {208, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_0_csid_irq"},
  {209, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_0_irq"},
  {210, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_1_csid_irq"},
  {211, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_1_irq"},
  {212, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_lite_csid_irq"},
  {213, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ife_lite_irq"},
  {214, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ipe_0_cdm_irq"},
  {215, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ipe_0_irq"},
  {216, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ipe_1_cdm_irq"},
  {217, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_ipe_1_irq"},
  {218, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_jpeg_0_irq"},
  {219, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_jpeg_3_irq"},
  {220, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "titan_lrme_irq"},
  {221, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, "na"},
  {INVALID_INTERRUPT, DALINTRCTRL_ENABLE_DEFAULT_SETTINGS, ""}
};


/*
 * pInterruptControllerConfigData
 * This is used by the interrupt controller driver to query platform specific data.
 */
InterruptPlatformDataType pInterruptControllerConfigData[] =
{
  {
    .pIRQConfigs = InterruptConfigs,
    .nMaxIRQ     = 222
  }
};