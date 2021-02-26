/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/sdm845/cvd_devcfg.c#4 $
  $Author: pwbldsvc $
*/

#include "mmpm.h"
#include "adsp_vpm_api.h"

#include "cvd_devcfg_i.h"
#include "vss_common_public_if.h"

/****************************************************************************
 * CVD DEVICE CONFIGURATION DATA                                            *
 ****************************************************************************/

/**
  This file along with the cvd_devcfg.xml (in the same folder) stores the
  following data:

  1. Voice use case <-> floor clock value table as follows:

    power dashboard use cases:

    media_id\topology_id    sm_ecns_v2  sm_ecns dm_fv5_r    dm_fv5_b_r  dm_fv5  dm_fv5_b
    evs-nb                  minSVS      NA      minSVS      minSVS      NA      NA
    evs-wb                  svs2        NA      minSVS      minSVS      NA      NA
    evs-swb                 svs_L1      NA      svs2        svs2        NA      NA
    evs-fb                  nom         NA      svs2        svs2        svs     svs
    nb                      minSVS      minSVS  svs2        minSVS      NA      NA
    wb                      svs2        minSVS  svs2        svs         NA      NA

    general use cases (this includes power dashboard use cases as well):
    
    media_id\topology_id    sm_ecns_v2  sm_ecns sm_fv5     dm_vpecns    dm_fv5_r    dm_fv5_b_r  dm_fv5  dm_fv5_b    qm_fp_v2  qm_fp_v2_r
    evs-nb                  minSVS      minSVS  svs2       NA           minSVS      minSVS      minSVS  minSVS      minSVS    minSVS
    evs-wb                  svs2        NA      svs        NA           minSVS      minSVS      minSVS  minSVS      svs2      svs2
    evs-swb                 svs_L1      NA      nom        NA           svs2        svs2        svs2    svs2        svs2      svs2
    evs-fb                  nom         NA      nom        NA           svs2        svs2        svs2    svs2        svs       svs2
    nb                      minSVS      minSVS  minSVS     minSVS       minSVS      minSVS      svs     svs2        minSVS    minSVS
    amr-wb                  minSVS      minSVS  svs        minSVS       svs2        svs2        svs     svs         svs2      svs2
    4gv-wb/eAMR             svs2        minSVS  svs_L1     svs2         svs2        svs         svs     svs         svs2      svs2

    Important Notes :
    1.  4gv-nb with sm_fv5 needs SVS2 to meet RTD limit. However, with minSVS it would have a maximum delay of 84.4 ms.
        This is 1.4 ms greater than limit.
    2.  4gv-nb with dm_fv5 (256 pt FFT) needs SVS2 to meet RTD limit. However, with minSVS it would have a maximum delay of 84.63 ms.
        This is 1.63 ms greater than limit. 
    3.  4gv-wb/eAMR with dm_vpencs needs SVS2 to meet RTD limit. However, with minSVS 4gv-wb and eAMR would have a maximum delay of 84.48 ms
        and 84.1ms respectively. This is 1.48 and 1.1 ms greater than limit. But, AMR-WB vocoder would operate at minSVS for the same topology.
    4.  4gv-wb/eAMR with sm_fv5 needs SVS_L1 to meet RTD limit. However, with SVS 4gv-wb and eAMR would have a maximum delay of 84.35 ms
        and 84.18ms respectively. This is 1.35 and 1.18 ms greater than limit. But, AMR-WB vocoder would operate at SVS for the same topology.
    5.  4gv-wb/eAMR with sm_ecns_v2 needs SVS2 to meet RTD limit. However, with minSVS 4gv-wb and eAMR would have a maximum delay of 85.38 ms
        and 85.01ms respectively. This is 2.38 and 2.01 ms greater than limit. But, AMR-WB vocoder would operate at minSVS for the same topology.
    6.  4gv-wb/eAMR with dm_fv5_b (256 pt FFT) needs SVS to meet RTD limit. However, with SVS2 4gv-wb and eAMR would have a maximum delay of 84.87 ms
        and 84.64ms respectively. This is 1.87 and 1.64 ms greater than limit. But, AMR-WB vocoder would operate at SVS2 for the same topology.


    Legend:
    dm_fv5_b_r --> DM_FLUENCEv5_BROADSIDE_REDUCED
    dm_fv5_r   --> DM_FLUENCEv5_REDUCED
    qm_fp_v2_r --> QM_FLUENCEPROV2_REDUCED

    Al the above 'REDUCED' topologies are similar to the below topologies except that 256 point FFT is being configured instead of
    default 512 point FFT for the EC-NS topologies. The QACT would host only the below topology ids but the back-end ACDB data would be
    a. 256 point FFT configuration if RTD has to be met
    b. 512 point FFT configuration if RTD is not of significance.

    dm_fv5_b --> DM_FLUENCEv5_BROADSIDE
    dm_fv5   --> DM_FLUENCEv5
    qm_fp_v2 --> QM_FLUENCEPROV2

    The clock levels minSVS, SVS2, SVS, Nom are defined below.

    All the above clock levels are decided based on assumption that only one voice stream and one vocproc session are present in the system.
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

#define CVD_DEVCFG_MINSVS  (288000000)

#define CVD_DEVCFG_SVS2    (460800000)

#define CVD_DEVCFG_SVS     (614400000)

#define CVD_DEVCFG_SVS_L1  (787200000)

#define CVD_DEVCFG_NOMINAL (940800000)

/* Definitions for Bandwidth in MBPS */
#define CVD_DEVCFG_BW_MBPS_45  ( 45 )
#define CVD_DEVCFG_BW_MBPS_70  ( 70 )
#define CVD_DEVCFG_BW_MBPS_130 ( 130 )
#define CVD_DEVCFG_BW_MBPS_150 ( 150 )

/* Macros local to this file */
#define CVD_UINT16_NA ( 0xFFFF )
#define CVD_UINT32_NA ( 0xFFFFFFFF )

/*
   All voice usecases are covered with the assumption that, one voice session ( MVM ) is 
   associated with only one Vocproc session and only one Stream session at any point of time."
*/
cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values =
  { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA };

cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[ ] =
  {
    /* All Quad Mic use cases. */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[ ] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> NB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> WB Stream -> NB/WB/SWB/FBP VocProc
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[ ] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> SWB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> FB Stream  -> NB/WB/SWB/FBP VocProc 
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB/WB(WVE) VocProc
        1 voice session -> WB Stream -> WB VocProc 
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB VocProc
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,    CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_4_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> WB VocProc
        1 voice session -> WB Stream -> WB VocProc
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,              CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    /*  Use cases covered for the below topology:
        1 voice session -> NB Stream -> NB VocProc
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
};

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_5_use_cases[ ] =
  {
    /* All other use cases */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[ ] =
  {
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_150 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_0_use_cases),
        cvd_devcfg_bw_level_0_use_cases
    },
    {
      {
        .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
        .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
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
         .bwValue.busBwValue = { (CVD_DEVCFG_BW_MBPS_45 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_3_use_cases),
      cvd_devcfg_bw_level_3_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (CVD_DEVCFG_BW_MBPS_70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_4_use_cases),
      cvd_devcfg_bw_level_4_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { (CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_5_use_cases),
      cvd_devcfg_bw_level_5_use_cases
    }
  };

cvd_devcfg_mmpm_bw_table_t cvd_devcfg_parser_bw_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_mmpm_bw_values), cvd_devcfg_mmpm_bw_values };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[ ] =
  {
    /*  Only EVS media stream use cases which needs minSVS are part of this classification
    */
    /*  Use case of NB with SM_ECNS/SM_ECNS_V2/QM_FP_V2

        1 voice session -> EVS-NB stream -> SM_ECNS. RTD: 83.01ms. 
        1 voice session -> EVS-NB stream -> SM_ECNS_V2. RTD: 82.15ms. Power Dashboard use case. Strict RTD 87ms.
        1 voice session -> EVS-NB stream -> QM_FP_V2. RTD: 96.7ms
        Allocate CVD_DEVCFG_MINSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,                CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2,             CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_QM_FLUENCE_PROV2,       CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },

    /*  Use case of EVS-NB/EVS-WB with DM_FV5/DM_FV5_BROADSIDE

        1 voice session -> EVS-NB/EVS-WB stream -> DM_FV5. RTD: 94.11ms (EVS-NB),100.06ms (EVS-WB)
        1 voice session -> EVS-NB/EVS-WB stream -> DM_FV5_BROADSIDE. RTD: 91.93ms (EVS-NB), 104.27ms (EVS-WB)
        Allocate CVD_DEVCFG_MINSVS
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[ ] =
  {
    /*  Only EVS media stream use cases which needs SVS2 are part of this classification
    */
    /*  Use case of NB with SM_FV5

        1 voice session -> EVS-NB Stream -> SM_FV5 VocProc. RTD: 84.16ms.
        Allocate CVD_DEVCFG_SVS2
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },

    /*  Use case of EVS-SWB/EVS-FB with DM_FV5/DM_FV5_BROADSIDE

        1 voice session -> EVS-SWB/EVS-FB stream -> DM_FV5. RTD: 112.86 (EVS-SWB), 113.29ms (EVS-FB)
        1 voice session -> EVS-SWB/EVS-FB stream -> DM_FV5_BROADSIDE. RTD: 113.29 (EVS-SWB), 113.71ms (EVS-FB)
        Allocate CVD_DEVCFG_SVS2
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },

    /*  Use case of EVS-FB with QM_FLUENCEPROV2

        1 voice session -> EVS-WB/EVS-SWB/EVS-FB Stream -> QM_FP_V2 VocProc. RTD: 93.4ms (EVS-WB), 110.54ms (EVS-WB), 119.09ms (EVS-FB).
        Allocate CVD_DEVCFG_SVS2
    */

    { 1, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[ ] =
  {
    /*  First classification is based on EVS media.
        Use cases covered for each of the below topologies:
        1 voice session -> EVS-WB Stream ->  SM_FV5 VocProc. RTD: 88.1ms
        Allocate CVD_DEVCFG_SVS
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[ ] =
  {
    /*  Only EVS media stream which need SVS_L1
    */
    /*  Use cases covered for each of the below topologies:

        1 voice session -> EVS-SWB Stream -> SM_ECNS_V2 VocProc. RTD: 88ms.        
        Allocate CVD_DEVCFG_SVS_L1
    */
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[ ] =
  {
    /*  Only EVS media stream which need Nominal
    */
    /*  Use cases covered for each of the below topologies:

        1 voice session -> EVS-SWB/EVS-FB Stream -> SM_FV5 VocProc VocProc. RTD: 96.5ms (EVS-SWB), 101ms (EVS-FB).
        Allocate CVD_DEVCFG_NOMINAL
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },

    /*  Use cases covered for each of the below topologies:        

        1 voice session -> EVS-FB Stream -> SM_ECNS_V2 VocProc VocProc. RTD: 90ms.
        Allocate CVD_DEVCFG_NOMINAL
    */
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_5_use_cases[ ] =
  {
    /*  Only non-EVS NB media stream and NB VocProc use cases which needs minSVS are part of this classification
    */
    /*  Use cases covered for each of the below topologies:

        1 voice session -> NB Stream -> NB SM_ECNS VocProc. RTD: 73.53ms
        1 voice session -> NB Stream -> NB SM_ECNS_V2 VocProc. RTD: 72.67ms
        1 voice session -> NB Stream -> NB SM_FV5 VocProc. RTD: 80.73ms and 84.4 ms (only 4GV-NB)
        1 voice session -> NB Stream -> NB DM_VPECNS VocProc. RTD: 73.9ms
        1 voice session -> NB Stream -> NB DM_FLUENCE VocProc. RTD: 81.17ms
        1 voice session -> NB Stream -> NB DM_FV5 VocProc. RTD: 81.17ms
        1 voice session -> NB Stream -> NB DM_FV5_BROADSIDE VocProc. RTD: 82.45ms
        1 voice session -> NB Stream -> NB QM_FV5 VocProc. RTD: 87.22ms
        Allocate CVD_DEVCFG_minSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,                CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,              CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_QM_FLUENCE_PROV2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },

    /*  Only non-EVS WB media stream and WB VocProc use cases which needs minSVS are part of this classification
    */
    /*  Use cases covered for each of the below topologies:

        1 voice session -> WB Stream -> WB SM_ECNS VocProc. RTD: 81.3ms
        1 voice session -> WB Stream -> WB DM_VPECNS VocProc. RTD: 80.3ms (AMR-WB), 84.48ms (4GV-WB), 84.1ms (eAMR)
        Please note 4GV-WB and eAMR would actually need SVS2. Please refer to table at start of file
        Allocate CVD_DEVCFG_minSVS
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,                CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,                CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_6_use_cases[ ] =
  {
    /*  Only non-EVS WB media stream and WB VocProc use cases which needs SVS2 are part of this classification
    */
    /*  Use cases covered for each of the below topologies:        

        1 voice session -> WB Stream -> WB SM_ECNS_V2 VocProc. RTD: 81.21ms (AMR-WB), 85.38ms (4GV-WB), 85.01ms (eAMR)
        Please note 4GV-WB and eAMR would actually need SVS. Please refer to table at start of file

        1 voice session -> WB Stream -> WB DM_FLUENCE VocProc. RTD: 82.24ms
        1 voice session -> WB Stream -> WB DM_FV5 VocProc. RTD: 82.24ms
        1 voice session -> WB Stream -> WB DM_FV5_BROADSIDE VocProc. RTD: 82.24ms (AMR-WB), 84.87ms (4GV-WB), 84.64ms (eAMR)
        Please note 4GV-WB and eAMR would actually need SVS. Please refer to table at start of file

        1 voice session -> WB Stream -> WB QM_FP_V2 VocProc. RTD: 88.4ms
        Allocate CVD_DEVCFG_SVS2
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },    
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_QM_FLUENCE_PROV2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_7_use_cases[ ] =
  {
    /*  Only non-EVS WB media stream and WB VocProc use cases which needs SVS are part of this classification
    */
    /*  Use cases covered for each of the below topologies:        

        1 voice session -> WB Stream -> WB SM_FV5 VocProc. RTD: 82.4ms (AMR-WB), 84.35ms (4GV-WB), 84.18ms (eAMR)
        Please note 4GV-WB and eAMR would actually need SVS_L1. Please refer to table at start of file

        Allocate CVD_DEVCFG_SVS

        This combination was always assigned to SVS2 even though the RTD was never met. Time to change the thought???
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };


static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_8_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> WB(WVE) VocProc.
        Allocate CVD_DEVCFG_SVS2
    */
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_NONE,         CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,      CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_9_use_cases[ ] =
  {
    /*  Next classification is based on DM topology for non-EVS media.
        Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream ->  (WB) DM_FV5 VocProc.
        1 voice session -> NB/WB Stream -> (WB) DM_FV5_BROADSIDE VocProc.
        Allocate CVD_DEVCFG_V2_SVS
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[ ] =
  {
    { CVD_DEVCFG_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases), cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_SVS_L1, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases), cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_NOMINAL, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_4_use_cases), cvd_devcfg_clock_level_4_use_cases },
    { CVD_DEVCFG_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_5_use_cases), cvd_devcfg_clock_level_5_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_6_use_cases), cvd_devcfg_clock_level_6_use_cases },
    { CVD_DEVCFG_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_7_use_cases), cvd_devcfg_clock_level_7_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_8_use_cases), cvd_devcfg_clock_level_8_use_cases },
    { CVD_DEVCFG_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_9_use_cases), cvd_devcfg_clock_level_9_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_MINSVS}, {CVD_DEVCFG_SVS2}, {CVD_DEVCFG_SVS}, {CVD_DEVCFG_SVS_L1}, {CVD_DEVCFG_NOMINAL} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_frequency_levels), cvd_devcfg_clock_frequency_levels  } ; 

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