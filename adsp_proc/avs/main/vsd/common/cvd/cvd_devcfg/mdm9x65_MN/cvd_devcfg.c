/*
  Copyright (C) 2015-2017 QUALCOMM Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/mdm9x65_MN/cvd_devcfg.c#2 $
  $Author: pwbldsvc $
*/

#include "adsp_vpm_api.h"
#include "mmpm.h"

#include "vss_private_if.h"

/****************************************************************************
 * CVD DEVICE CONFIGURATION DATA                                            *
 ****************************************************************************/

/**
  This file along with the cvd_devcfg.xml (in the same folder) stores the
  following data:

  1. Voice use case <-> floor clock value table as follows:

    power dashboard use cases:

    media_id\topology_id    vptx_none
    evs-nb                  svs2
    evs-wb                  svs2
    evs-swb                 svs_L1
    evs-fb                  svs_L1  
    nb                      minSVS
    4gv-nb                  svs2(with extra 0.5ms RTD hit. svs with no RTD hit)
    amr-wb                  svs2
    4gv-wb                  svs
    4gv-nw                  svs

    The clock levels minSVS, SVS2, SVS, SVS_L1, Nom are defined below.

    All the above clock levels are decided based on assumption that only one voice stream and one vocproc session are present in the system.

  2. MMPM core information which is required for MMPM registration:
     - Core ID = MMPM_CORE_ID_LPASS_ADSP
     - Instance ID = MMPM_CORE_INSTANCE_0

  3. MMPM Bus Bandwidth requirement:
     - Number of bus bandwidth requirement = 1
     - Bus route master port = MMPM_BW_PORT_ID_ADSP_MASTER
     - Bus route slave port = MMPM_BW_PORT_ID_DDR_SLAVE
     - Usage = varies based on usecase
     - Usage percentage = 100
     - Usage type = MMPM_BW_USAGE_LPASS_DSP

   4. Sleep latency in microsends, which means during voice call, the ADSP
      should not go to sleep (power collapse) unless it can wake up within
      this latency. (this information is directly stored in the
      cvd_devcfg.xml).
      - 25

   5. Number of clock cycles required to execute 1000 instruction packets (
      this information is directly stored in the cvd_devcfg.xml).
      - 4000
*/

/* Use case:
   { Number of voice sessions,
     Number of NB streams,
     Number of WB streams,
     Number of SWB streams,
     Number of FB plus streams,
     Number of NB vocprocs,
     Number of WB vocprocs,
     Number of SWB vocprocs,
     Number of FB plus vocprocs,
     Tx topology ID,
     Rx topology ID,
     Media ID,
     VFR mode,
     Number of timewarping streams
   }
*/

#define CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US ( 1000 )
  /**<
   * Default 1ms safety margin in the VpTx and VpRx timing offsets that CVD
   * sets on VCP. This is needed to take care of thread preemption delay.
   */

#define CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US ( 1500 )
  /**<
   * Default 1.5ms safety margin in the Venc and Vdec timing offsets that CVD
   * sets on VCP. This is needed to take care of thread preemption delay.
   */

#define CVD_DEVCFG_MVM_EVS_ENCODER_PREMPTION_SAFETY_MARGIN_US ( 4100 )
  /**<
   * Additional 2.6ms (in addition to 1.5ms) safety margin in the Venc timing
   * offsets that CVD sets on VCP. This is needed to take care of higher 
   * priority modem thread preemption delay incase of EVS SWB & FB call.
   */

#define CVD_DEVCFG_MVM_AMRWB_ENCODER_PREMPTION_SAFETY_MARGIN_US ( 4500 )
  /**<
   * Additional 3.0ms (in addition to 1.5ms) safety margin in the Venc timing
   * offsets that CVD sets on VCP. This is needed to take care of higher 
   * priority modem thread preemption delay incase of LTE call. This latency 
   * does not impact RTD for CS call.
   */

#define CVD_DEVCFG_SIZEOF_ARRAY(X) ( sizeof(X) / sizeof( (X)[0] ) )

/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_CLK_HZ_minSVS    ( 230000000 )
#define CVD_DEVCFG_CLK_HZ_SVS2      ( 384000000 )
#define CVD_DEVCFG_CLK_HZ_SVS       ( 555000000 )
#define CVD_DEVCFG_CLK_HZ_SVS_L1    ( 656000000 )
#define CVD_DEVCFG_CLK_HZ_NOM       ( 880000000 )

/* Definitions for Bandwidth in MBPS */
#define CVD_DEVCFG_BW_MBPS_40  ( 40 )
#define CVD_DEVCFG_BW_MBPS_70  ( 70 )
#define CVD_DEVCFG_BW_MBPS_130 ( 130 )
#define CVD_DEVCFG_BW_MBPS_150 ( 150 )

/* Macros local to this file */
#define CVD_UINT16_NA ( 0xFFFF )
#define CVD_UINT32_NA ( 0xFFFFFFFF )

cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values =
  { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA };

/* EVS-NB, EVS-WB with VPTX_NONE would need SVS2 frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[ ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }    
  };

/* EVS-SWB, EVS-FB with VPTX_NONE would need SVS_L1 frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[ ] =
  {
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

/* all other EVS scenarios would need SVS frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[ ] =
  {    
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 0, 1, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 0, 1, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 1, 0, 1, 0, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 1, 0, 0, 0, 0, 1, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 0, 1, 1, 0, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 0, 1, 0, 1, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

/* 4GV-NB with VPTX_NONE would need SVS2 frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[ ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_4GV_NB, CVD_UINT16_NA }
  };

/* All NB, except 4GV-NB, with VPTX_NONE would need minSVS frequency */  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[ ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

/* 4GV-WB, with VPTX_NONE would need SVS frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_5_use_cases[ ] =
  {
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_4GV_WB, CVD_UINT16_NA }
  };

/* 4GV-NW, with VPTX_NONE would need SVS frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_6_use_cases[ ] =
  {
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_4GV_NW, CVD_UINT16_NA }
  };

/* AMR-WB, with VPTX_NONE would need SVS2 frequency */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_7_use_cases[ ] =
  {
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, VSS_MEDIA_ID_AMR_WB, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[ ] =
  {
    { CVD_DEVCFG_CLK_HZ_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS_L1, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases),  cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS_L1, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases),  cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_CLK_HZ_minSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_4_use_cases), cvd_devcfg_clock_level_4_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_5_use_cases),  cvd_devcfg_clock_level_5_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_6_use_cases),  cvd_devcfg_clock_level_6_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_7_use_cases), cvd_devcfg_clock_level_7_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_CLK_HZ_minSVS}, {CVD_DEVCFG_CLK_HZ_SVS2}, {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_SVS_L1}, {CVD_DEVCFG_CLK_HZ_NOM} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_frequency_levels), cvd_devcfg_clock_frequency_levels  } ; 


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB/WB(WVE) VocProc
        1 voice session -> WB Stream -> WB VocProc 
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }

  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> NB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> WB Stream -> NB/WB/SWB/FBP VocProc
    */
    { 1, 0xFFFF, 0xFFFF, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> SWB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> FB Stream  -> NB/WB/SWB/FBP VocProc 
    */
    { 1, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };  

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[] =
  {
    /* All other use cases */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[] =
  {
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_40 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_0_use_cases),
      cvd_devcfg_bw_level_0_use_cases
    },
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
        .bwValue.busBwValue = { (CVD_DEVCFG_BW_MBPS_70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_1_use_cases),
      cvd_devcfg_bw_level_1_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_2_use_cases),
      cvd_devcfg_bw_level_2_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_3_use_cases),
      cvd_devcfg_bw_level_3_use_cases
    }
  };

cvd_devcfg_mmpm_bw_table_t cvd_devcfg_parser_bw_table = 
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_mmpm_bw_values), cvd_devcfg_mmpm_bw_values };


/********************************************************************
  VOICE PROCESSING PREEMTION MARGIN CONFIGURATION
 ********************************************************************/

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_preemption_safety_margins_level_0_use_cases[ ] =
  {
    {
      CVD_UINT16_NA,
      CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA
    }
  };

/* EVS SWB and FB scenarios would need an extra margin for MDM targets */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_preemption_safety_margins_level_1_use_cases[ ] =
  {
    {
      1,
      0, 0, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA
    }
  };

/* AMR-WB scenarios would need an extra margin for LTE cnd CS call.  */
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_preemption_safety_margins_level_2_use_cases[ ] =
  {
    {
      1,
      CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA,
      CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_AMR_WB, CVD_UINT16_NA
    }
  };


static cvd_devcfg_preemption_safety_margins_level_t cvd_devcfg_preemption_safety_margins_levels[ ] =
  {
    {
      {
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_AMRWB_ENCODER_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US
      }, 
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_level_2_use_cases), 
      cvd_devcfg_preemption_safety_margins_level_2_use_cases
    },

    {
      {
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_EVS_ENCODER_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US
      }, 
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_level_1_use_cases), 
      cvd_devcfg_preemption_safety_margins_level_1_use_cases
    },

    { 
      {
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US,
        CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_level_0_use_cases),  
      cvd_devcfg_preemption_safety_margins_level_0_use_cases
    }
  };


cvd_devcfg_preemption_safety_margins_table_t cvd_devcfg_preemption_safety_margins_table =
  {
    CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_levels),
    cvd_devcfg_preemption_safety_margins_levels
  };

/* Undefine macros local to this file. */
#undef CVD_UINT16_NA 
#undef CVD_UINT32_NA