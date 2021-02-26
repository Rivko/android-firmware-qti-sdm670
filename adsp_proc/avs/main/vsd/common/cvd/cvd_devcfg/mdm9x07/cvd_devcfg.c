/*
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c#2 $
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

  1. Below table shows the Voice use case as media ID and Tx topology combination with floor_clock:
  
  For SOFT VFR use cases such as VOLTE Calls the following table applies.  
     Topology/Media  | NB Voc | WB Vocproc 
     _________________________________
     NONE            | SVS    | SVS 
     SM_ECNS         | SVS    | SVS 
     SM_ECNS V2      | SVS    | SVS 
     
  The following table applies for both SOFT and HARD VFR use cases.
     Topology/Media  | NB Voc | WB Vocproc 
     _________________________________
     NONE            | SVS    | SVSPlus 
     SM_ECNS         | SVS    | SVSPlus 
     SM_ECNS V2      | SVS    | SVSPlus 
     SM_FLUENCEV5    | SVS    | NOM
     DM_FLUENCE      | SVS    | NOM
     DM_VPECNS       | SVS    | SVSPlus
     DM_F_V5         | NOM    | NOM 
     DM_F_V5_BRO     | NOM    | NOM 
     QM_F_PRO        | NOM    | NOM

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

  4. Sleep latency in microseconds, which means during voice call, the ADSP
     should not go to sleep (power collapse) unless it can wake up within
     this latency. (this information is directly stored in the
     cvd_devcfg.xml).
     - 25

  5. Number of clock cycles required to execute 1000 instruction packets (
     this information is directly stored in the cvd_devcfg.xml).
     - 9600

  6. Clock table
     ---------------------------
     | CLOCK LEVEL | VALUE(MHz)|
     ---------------------------
     | SVS         | 384.00    |
     ---------------------------
     | SVS+        | 480.00    |
     ---------------------------
     | NOM         | 576.00    |
     ---------------------------
     | NOM+        | 614.40    |
     ---------------------------
     | TURBO       | 691.20    |
     ---------------------------

  7. Use case:
      { 
        Number of voice session,
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

#define CVD_DEVCFG_SIZEOF_ARRAY(X) ( sizeof(X) / sizeof( (X)[0] ) )

/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_CLK_HZ_SVS      ( 384000000 )
#define CVD_DEVCFG_CLK_HZ_SVSPLUS  ( 480000000 )
#define CVD_DEVCFG_CLK_HZ_NOM      ( 576000000 )
#define CVD_DEVCFG_CLK_HZ_NOMPLUS  ( 614000000 )
#define CVD_DEVCFG_CLK_HZ_TURBO    ( 691000000 )

/* Definitions for Bandwidth in MBPS */
#define CVD_DEVCFG_BW_MBPS_40  ( 40 )
#define CVD_DEVCFG_BW_MBPS_50  ( 50 )
#define CVD_DEVCFG_BW_MBPS_70  ( 70 )
#define CVD_DEVCFG_BW_MBPS_80  ( 80 )
#define CVD_DEVCFG_BW_MBPS_130 ( 130 )

/* Macros local to this file */
#define CVD_UINT16_NA ( 0xFFFF )
#define CVD_UINT32_NA ( 0xFFFFFFFF )
/*
   All voice usecases are covered with the assumption that, one voice session ( MVM ) is 
   associated with only one Vocproc session and only one Stream session at any point of time.
*/
cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values =
  { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[] =
  {    
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream -> NB/WB VocProc with SOFT VFR for VOLTE calls
        Clock: CVD_DEVCFG_CLK_HZ_SVS.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,             CVD_UINT32_NA, CVD_UINT32_NA, VSS_ICOMMON_VFR_MODE_SOFT },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, CVD_UINT32_NA, VSS_ICOMMON_VFR_MODE_SOFT },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, CVD_UINT32_NA, VSS_ICOMMON_VFR_MODE_SOFT },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB VocProc.
        Clock: CVD_DEVCFG_CLK_HZ_SVS.
    */
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_NONE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream -> NB/WB VocProc 
        Clock: CVD_DEVCFG_CLK_HZ_SVSPLUS.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,  CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_VPECNS,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream -> NB/WB VocProc 
        Clock: CVD_DEVCFG_CLK_HZ_NOM.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_VPECNS,              CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_QM_FLUENCE_PROV2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[] =
  {
    /* All other use cases 
       Clock: CVD_DEVCFG_CLK_HZ_NOM.
    */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[] =
  {
    { CVD_DEVCFG_CLK_HZ_SVS,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVSPLUS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases), cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases), cvd_devcfg_clock_level_3_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[] = { {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_SVSPLUS}, {CVD_DEVCFG_CLK_HZ_NOM}, {CVD_DEVCFG_CLK_HZ_NOMPLUS}, {CVD_DEVCFG_CLK_HZ_TURBO} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_frequency_levels), cvd_devcfg_clock_frequency_levels  } ; 


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB/WB(WVE) VocProc
        1 voice session -> WB Stream -> WB VocProc 
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,    CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB VocProc
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[] =
  {
   /*  Use cases covered for each of the below topologies:
       1 voice session -> NB Stream -> WB(WVE) VocProc
       1 voice session -> WB Stream -> WB VocProc 
   */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[] =
  {
   /*  Use cases covered for each of the below topologies:
       1 voice session -> NB/WB Stream -> WB VocProc
   */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   /*  Use cases covered for the below topology:
       1 voice session -> NB/WB Stream -> NB/WB VocProc
   */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   /* All Quad Mic use cases. */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
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
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_50 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_1_use_cases),
      cvd_devcfg_bw_level_1_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_80 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
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

/* Undefine macros local to this file. */
#undef CVD_UINT16_NA 
#undef CVD_UINT32_NA 

