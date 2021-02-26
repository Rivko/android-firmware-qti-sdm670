/*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: 
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

     Tx Topology,     | 1 voice session   | 1 voice session     | Any other number     |
     Media ID and     | with 1 NB stream  | with 1 WB/WB+       | of voice sessions    |
     VFR mode         | and/or 1 NB       | stream and/or       | or any other number  |
                      | vocproc           | 1 WB/WB+ vocproc    | of streams/vocprocs  |
     -----------------------------------------------------------------------------------
     NONE +           |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any  VFR mode    |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS +        |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_ECNS_V2 +     |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FLUENCE +     |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_FLUENCEV5 +   |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     SM_CUSTOM TOPOs+ |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_VPECNS +      |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FLUENCEV5 +   |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_CUSTOM TOPOs+ |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     DM_FV5 BROADSIDE +| 403.2MHz         |  403.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_FLUENCE_PRO + |  403.2MHz         |  595.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_FLUENCE_PROV2 |  403.2MHz         |  595.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     QM_CUSTOM TOPOs +|  403.2MHz         |  595.2MHz           |  595.2MHz            |
     any non-EVS +    |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------
     Any Tx Topology+ |  403.2MHz         |  403.2MHz           |  595.2MHz            |
     EVS media +      |                   |                     |                      |
     any VFR mode     |                   |                     |                      |
     -----------------------------------------------------------------------------------

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
     - 4000

  6. Clock table
     ---------------------------
     | CLOCK LEVEL | VALUE(MHz)|
     ---------------------------
     | SVS         | 403.20    |
     ---------------------------
     | SVS+        | 518.40    |
     ---------------------------
     | NOM         | 595.20    |
     ---------------------------
     | NOM+        | 652.80    |
     ---------------------------
     | TURBO       | 710.40    |
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

#define CVD_DEVCFG_SIZEOF_ARRAY(X) ( sizeof(X) / sizeof( (X)[0] ) )

/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_CLK_HZ_SVS      ( 403200000 )
#define CVD_DEVCFG_CLK_HZ_SVSPLUS  ( 518400000 )
#define CVD_DEVCFG_CLK_HZ_NOM      ( 595200000 )
#define CVD_DEVCFG_CLK_HZ_NOMPLUS  ( 652800000 )
#define CVD_DEVCFG_CLK_HZ_TURBO    ( 710400000 )

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
    /*  Use cases covered for EVS Media Type:
        1 voice session -> NB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> WB Stream -> NB/WB/SWB/FBP VocProc
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> SWB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> FB Stream  -> NB/WB/SWB/FBP VocProc 
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[] =
  {
    /*  All NB use cases except EVS. */
    { 1, 1, 0, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> WB Stream -> NB/WB VocProc 
    */
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,                   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,                CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_SM_ECNS_1,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_SM_ECNS_2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_SM_ECNS_3,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_VPECNS,              CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_DM_ECNS_2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_CUSTOM_DM_ECNS_3,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[] =
  {
    /* All Quad Mic use cases. */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_3, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[] =
  {
    /* All other use cases */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[] =
  {
    { CVD_DEVCFG_CLK_HZ_SVS,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases), cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVSPLUS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases), cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,     CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_4_use_cases), cvd_devcfg_clock_level_4_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_SVSPLUS}, {CVD_DEVCFG_CLK_HZ_NOM}, {CVD_DEVCFG_CLK_HZ_NOMPLUS}, {CVD_DEVCFG_CLK_HZ_TURBO} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_frequency_levels), cvd_devcfg_clock_frequency_levels  } ; 


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> NB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> WB Stream -> NB/WB/SWB/FBP VocProc
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> SWB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> FB Stream  -> NB/WB/SWB/FBP VocProc 
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[] =
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
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_SM_ECNS_3, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_CUSTOM_DM_ECNS_3, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[] =
  {
   /*  Use cases covered for each of the below topologies:
       1 voice session -> NB Stream -> WB(WVE) VocProc
       1 voice session -> WB Stream -> WB VocProc 
   */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_SM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_SM_ECNS_2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_SM_ECNS_3, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_4_use_cases[] =
  {
   /*  Use cases covered for each of the below topologies:
       1 voice session -> NB/WB Stream -> WB VocProc
   */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_CUSTOM_DM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   /*  Use cases covered for the below topology:
       1 voice session -> NB/WB Stream -> NB/WB VocProc
   */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   /* All Quad Mic use cases. */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_1, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_CUSTOM_QM_ECNS_3, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_5_use_cases[] =
  {
    /* All other use cases */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[] =
  {
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_0_use_cases),
      cvd_devcfg_bw_level_0_use_cases
    },
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
        .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_1_use_cases),
      cvd_devcfg_bw_level_1_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_40 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_2_use_cases),
      cvd_devcfg_bw_level_2_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_50 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_3_use_cases),
      cvd_devcfg_bw_level_3_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_80 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_4_use_cases),
      cvd_devcfg_bw_level_4_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_5_use_cases),
      cvd_devcfg_bw_level_5_use_cases
    }
  };

cvd_devcfg_mmpm_bw_table_t cvd_devcfg_parser_bw_table = 
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_mmpm_bw_values), cvd_devcfg_mmpm_bw_values };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_preemption_safety_margins_level_0_use_cases[ ] =
  {
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA}
  };

static cvd_devcfg_preemption_safety_margins_level_t cvd_devcfg_preemption_safety_margins_levels[ ] =
    {
        { {CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,CVD_DEVCFG_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US,CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US,CVD_DEVCFG_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US}, 
            CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_level_0_use_cases), 
            cvd_devcfg_preemption_safety_margins_level_0_use_cases }
    };

cvd_devcfg_preemption_safety_margins_table_t cvd_devcfg_preemption_safety_margins_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_preemption_safety_margins_levels), cvd_devcfg_preemption_safety_margins_levels };
  
/* Undefine macros local to this file. */
#undef CVD_UINT16_NA 
#undef CVD_UINT32_NA 

