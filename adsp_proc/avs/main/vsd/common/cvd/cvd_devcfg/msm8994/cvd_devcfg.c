/*
  Copyright (c) 2013-2015 , 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/msm8994/cvd_devcfg.c#3 $
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

  1. Voice use case <-> clock value table as follows: 
     
     Tx Topology and  | 1 voice session   | 1 voice session     | Any other number     |
     VFR mode         | with 1 NB stream  | with 1 WB/WB+       | of voice sessions    |
                      | and/or 1 NB       | stream and/or       | or any other number  |
                      | vocproc           | 1 WB/WB+ vocproc    | of streams/vocprocs  |
     -----------------------------------------------------------------------------------
     ANY +            |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     SOFT VFR         |                   |                     |                      |                      
     -----------------------------------------------------------------------------------
     NONE +           |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS +        |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS +        |  83MHz/HW thread  |  83MHz/HW thread    |  147MHz/HW thread    |
     SOFT VFR mode    |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS_V2 +     |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FLUENCE +     |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |
     -----------------------------------------------------------------------------------
     SM_FLUENCEV5 +   |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |
     -----------------------------------------------------------------------------------
     SM_CUSTOM TOPOs+ |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |
     -----------------------------------------------------------------------------------
     DM_VPECNS +      |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |
     -----------------------------------------------------------------------------------
     DM_FLUENCEV5 +   |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_CUSTOM TOPOs+ |  83MHz/HW thread  |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_FLUENCE_PRO + |  147MHz/HW thread |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_CUSTOM TOPOs+ |  147MHz/HW thread |  147MHz/HW thread   |  147MHz/HW thread    |
     any VFR mode     |                   |                     |                      |



  2. MMPM core information which is required for MMPM registration:
     - Core ID = MMPM_CORE_ID_LPASS_ADSP
     - Instance ID = MMPM_CORE_INSTANCE_0

  3. MMPM Bus Bandwidth requirement:
     - Number of bus bandwidth requirement = 1
     - Bus route master port = MMPM_BW_PORT_ID_ADSP_MASTER
     - Bus route slave port = MMPM_BW_PORT_ID_DDR_SLAVE
     - Usage = ( 100 << 20 ) Bytes per second
     - Usage percentage = 30
     - Usage type = MMPM_BW_USAGE_LPASS_DSP

   4. Sleep latency in microsends, which means during voice call, the ADSP
      should not go to sleep (power collapse) unless it can wake up within
      this latency. (this information is directly stored in the 
      cvd_devcfg.xml).
      - 20

   5. Number of clock cycles required to execute 1000 instruction packets (
      this information is directly stored in the cvd_devcfg.xml).
      - 1800
*/

/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_NUM_CLK_FREQ_LEVELS ( 4 )
#define CVD_DEVCFG_CLK_HZ_SVS2   ( 166750000 )
#define CVD_DEVCFG_CLK_HZ_SVS   ( 333500000 )
#define CVD_DEVCFG_CLK_HZ_NOM   ( 667000000 )
#define CVD_DEVCFG_CLK_HZ_TURBO ( 800000000 )

cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values = 
  { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[ 15 ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE,         0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,      0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2,      0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,   0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_1, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_2, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_3, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,    0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_2, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_3, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,      0xFFFFFFFF, VSS_ICOMMON_VFR_MODE_SOFT }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[ 3 ] = 
  {
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, VSS_ICOMMON_VFR_MODE_SOFT },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, VSS_ICOMMON_VFR_MODE_SOFT },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, VSS_ICOMMON_VFR_MODE_SOFT },
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[ 1 ] = 
  {  
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[ 3 ] = 
  {
    { 83000000, 15, cvd_devcfg_clock_level_0_use_cases },
    { 147000000, 3, cvd_devcfg_clock_level_1_use_cases },
    { 147000000, 1, cvd_devcfg_clock_level_2_use_cases } 
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table = 
  { 3, cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_CLK_HZ_SVS2}, {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_NOM}, {CVD_DEVCFG_CLK_HZ_TURBO} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_NUM_CLK_FREQ_LEVELS, cvd_devcfg_clock_frequency_levels  } ; 


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info = 
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[ 1 ] = 
  {
    { 
      { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
      { ( 100 << 20 ), 30, MMPM_BW_USAGE_LPASS_DSP } 
    }
  };

cvd_devcfg_mmpm_bw_requirement_t cvd_devcfg_mmpm_bw_requirement = 
  { 1, cvd_devcfg_mmpm_bw_values };

