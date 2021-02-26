/*
  Copyright (c) 2015-2016 , 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header:
  $Author: pwbldsvc $
*/

#include "mmpm.h"
#include "adsp_vpm_api.h"

#include "vss_private_if.h"

/****************************************************************************
 * CVD DEVICE CONFIGURATION DATA                                            *
 ****************************************************************************/

/**
  This file along with the cvd_devcfg.xml (in the same folder) stores the
  following data:

  1. Voice use case <-> floor clock value table as follows:

     Tx Topology,     | 1 voice session   | 1 voice session     | Any other number     |
     Media ID and     | with 1 NB stream  | with 1 WB/WB+       | of voice sessions    |
     VFR mode         | and/or 1 NB       | stream and/or       | or any other number  |
                      | vocproc           | 1 WB/WB+ vocproc    | of streams/vocprocs  |
     -----------------------------------------------------------------------------------
     NONE +           |  384MHz           |  384MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any  VFR mode    |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS +        |  384MHz           |  384MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS_V2 +     |  384MHz           |  384MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FLUENCE +     |  384MHz           |  480MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_FLUENCEV5 +   |  384MHz           |  384MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_CUSTOM TOPOs+ |  384MHz           |  384MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_VPECNS +      |  384MHz           |  480MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FLUENCEV5 +   |  384MHz           |  480MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_CUSTOM TOPOs+ |  384MHz           |  480MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FV5 BROADSIDE +| 480MHz           |  691MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_FLUENCE_PRO + |  691MHz           |  691MHz             |  691MHz              |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_FLUENCE_PROV2 |  691MHz           |  691MHz             | 691MHz               |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_CUSTOM TOPOs +|  691MHz           |  691MHz             | 691MHz               |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     Any Tx Topology+ |  691MHz           |  691MHz             | 691MHz               |
     EVS media +      |                   |                     |                      |		 
     any VFR mode     |                   |                     |                      |


  2. MMPM core information which is required for MMPM registration:
     - Core ID = MMPM_CORE_ID_LPASS_ADSP
     - Instance ID = MMPM_CORE_INSTANCE_0

  3. MMPM Bus Bandwidth requirement:
     - Number of bus bandwidth requirement = 1
     - Bus route master port = MMPM_BW_PORT_ID_ADSP_MASTER
     - Bus route slave port = MMPM_BW_PORT_ID_DDR_SLAVE
     - Usage depends on use case
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
   { Number of voice session,
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
     VFR mode 
   } 
*/

#define CVD_DEVCFG_NUM_CLK_LEVELS ( 6 )
#define CVD_DEVCFG_NUM_CLK_L0_USECASE ( 1 )
#define CVD_DEVCFG_NUM_CLK_L1_USECASE ( 8 )
#define CVD_DEVCFG_NUM_CLK_L2_USECASE ( 8 )
#define CVD_DEVCFG_NUM_CLK_L3_USECASE ( 11 )
#define CVD_DEVCFG_NUM_CLK_L4_USECASE ( 4 )
#define CVD_DEVCFG_NUM_CLK_L5_USECASE ( 1 )

#define CVD_DEVCFG_NUM_BW_LEVELS ( 6 )
#define CVD_DEVCFG_NUM_BW_L0_USECASE ( 1 )
#define CVD_DEVCFG_NUM_BW_L1_USECASE ( 8 )
#define CVD_DEVCFG_NUM_BW_L2_USECASE ( 8 )
#define CVD_DEVCFG_NUM_BW_L3_USECASE ( 11 )
#define CVD_DEVCFG_NUM_BW_L4_USECASE ( 4 )
#define CVD_DEVCFG_NUM_BW_L5_USECASE ( 1 )

/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_NUM_CLK_FREQ_LEVELS ( 5 )
#define CVD_DEVCFG_CLK_HZ_SVS        ( 384000000 )
#define CVD_DEVCFG_CLK_HZ_SVS_PLUS   ( 480000000 )
#define CVD_DEVCFG_CLK_HZ_NOM        ( 576000000 )
#define CVD_DEVCFG_CLK_HZ_NOM_PLUS   ( 614000000 )
#define CVD_DEVCFG_CLK_HZ_TURBO      ( 691000000 )

cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values =
  { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[ CVD_DEVCFG_NUM_CLK_L0_USECASE ] =
  {
    { 1, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, VPM_TX_QM_FLUENCE_PROV2,          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[ CVD_DEVCFG_NUM_CLK_L1_USECASE ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },	
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[ CVD_DEVCFG_NUM_CLK_L2_USECASE ] =
  {
    { 1, 0, 0, 1, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },	
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 1, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },	
    { 1, 0, 0, 0, 1, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },	
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[ CVD_DEVCFG_NUM_CLK_L3_USECASE ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE,             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_NONE,             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5,     0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5,     0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,       0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,     0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[ CVD_DEVCFG_NUM_CLK_L4_USECASE ] =
  {
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,           0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
};

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_5_use_cases[ CVD_DEVCFG_NUM_CLK_L5_USECASE ] =
  {
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[ CVD_DEVCFG_NUM_CLK_LEVELS ] =
  {
    { CVD_DEVCFG_CLK_HZ_TURBO, CVD_DEVCFG_NUM_CLK_L0_USECASE, cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS_PLUS, CVD_DEVCFG_NUM_CLK_L1_USECASE, cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_CLK_HZ_TURBO, CVD_DEVCFG_NUM_CLK_L2_USECASE, cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS, CVD_DEVCFG_NUM_CLK_L3_USECASE, cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS_PLUS, CVD_DEVCFG_NUM_CLK_L4_USECASE, cvd_devcfg_clock_level_4_use_cases },
    { CVD_DEVCFG_CLK_HZ_TURBO, CVD_DEVCFG_NUM_CLK_L5_USECASE, cvd_devcfg_clock_level_5_use_cases }
  }; 

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_NUM_CLK_LEVELS, cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ CVD_DEVCFG_NUM_CLK_FREQ_LEVELS ] = { {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_SVS_PLUS}, {CVD_DEVCFG_CLK_HZ_NOM}, {CVD_DEVCFG_CLK_HZ_NOM_PLUS}, {CVD_DEVCFG_CLK_HZ_TURBO} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_NUM_CLK_FREQ_LEVELS, cvd_devcfg_clock_frequency_levels  } ; 


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[ CVD_DEVCFG_NUM_BW_L0_USECASE ] =
  {
    { 1, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, VPM_TX_QM_FLUENCE_PROV2,          0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[ CVD_DEVCFG_NUM_BW_L1_USECASE ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[ CVD_DEVCFG_NUM_BW_L2_USECASE ] =
  {
    { 1, 0, 0, 1, 0, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 1, 0, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 1, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0xFFFFFFFF, 0xFFFFFFFF, VSS_MEDIA_ID_EVS, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[ CVD_DEVCFG_NUM_BW_L3_USECASE ] =
  {
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_NONE,         0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_NONE,         0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2,   0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,   0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,   0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_4_use_cases[ CVD_DEVCFG_NUM_BW_L4_USECASE ] =
  {
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,           0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_5_use_cases[ CVD_DEVCFG_NUM_BW_L5_USECASE ] =
  {  
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF }
  };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[ CVD_DEVCFG_NUM_BW_LEVELS ] =
  {
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( 150 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L0_USECASE,
        cvd_devcfg_bw_level_0_use_cases
    },
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
        .bwValue.busBwValue = { ( 70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L1_USECASE,
        cvd_devcfg_bw_level_1_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L2_USECASE,
      cvd_devcfg_bw_level_2_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (45 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L3_USECASE,
      cvd_devcfg_bw_level_3_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L4_USECASE,
      cvd_devcfg_bw_level_4_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_NUM_BW_L5_USECASE,
      cvd_devcfg_bw_level_5_use_cases
    }
  };

cvd_devcfg_mmpm_bw_table_t cvd_devcfg_parser_bw_table = 
  { CVD_DEVCFG_NUM_BW_LEVELS, cvd_devcfg_mmpm_bw_values };
