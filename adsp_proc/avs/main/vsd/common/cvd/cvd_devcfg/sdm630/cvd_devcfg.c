/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/sdm630/cvd_devcfg.c#3 $
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

    power dashboard use cases:

	media_id\topology_id 	sm_ecns_v2	sm_ecns	dm_fv5_r	dm_fv5_b_r	dm_fv5	dm_fv5_b
	evs-nb					minSVS		minSVS  svs_plus		minSVS		NA	    NA
	evs-wb					svs2		minSVS	svs_plus		svs2		NA	    NA
	evs-swb					svs_plus	NA		nom		svs2		NA	    NA
	evs-fb					nom			NA		nom		svs2		NA		NA
	nb						minSVS		minSVS	svs2		svs2		NA	    NA
	wb						svs2		svs2	svs			svs			NA	    NA

	general use cases (this includes power dashboard use cases as well):
	
    media_id\topology_id 	sm_ecns_v2	sm_ecns	sm_fv5	 dm_vpecns	dm_fv5_r	dm_fv5_b_r	dm_fv5	dm_fv5_b	qm_fp_v2  qm_fp_v2_r
	evs-nb					minSVS		minSVS	SVS2	 NA			svs_plus		minSVS		NA		NA			NA	  	  svs2
	evs-wb					svs2		minSVS	svs		 NA			svs_plus		svs2		NA		NA			NA	  	  svs2
	evs-swb					svs_plus	NA		nom		 NA			nom		svs2		NA		NA			NA		  svs
	evs-fb					nom			NA		nom		 NA			nom		svs2		NA		NA			NA		  svs
	nb						minSVS		minSVS	svs2	 minSVS		svs2		svs2		NA		NA			NA	  	  svs2
	wb						svs2		svs2	svs2     svs2		svs			svs			NA		NA			NA	  	  svs2

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

	The clock levels minSVS, SVS2, SVS, SVS_plus, Nom, Nom_plus, Turbo are defined below.

        All the above clock levels are decided based on assumption that only one voice stream and one vocproc session are present in the system.
   
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
      - 3700
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

/* Definitions for Bandwidth in MBPS */
#define CVD_DEVCFG_BW_MBPS_45  ( 45 )
#define CVD_DEVCFG_BW_MBPS_70  ( 70 )
#define CVD_DEVCFG_BW_MBPS_130 ( 130 )
#define CVD_DEVCFG_BW_MBPS_150 ( 150 )


/* Definitions for ADSP clock in Hz */
#define CVD_DEVCFG_MINSVS        (211200000)
#define CVD_DEVCFG_SVS2          (326400000)
#define CVD_DEVCFG_SVS           (499200000)
#define CVD_DEVCFG_SVS_PLUS      (614400000)
#define CVD_DEVCFG_NOMINAL       (748800000)
#define CVD_DEVCFG_NOMINAL_PLUS  (864000000)
#define CVD_DEVCFG_TURBO         (998400000)

/* Macros local to this file */
#define CVD_UINT16_NA ( 0xFFFF )
#define CVD_UINT32_NA ( 0xFFFFFFFF )
/*
   All voice usecases are covered with the assumption that, one voice session ( MVM ) is
   associated with only one VocProc session and only one Stream session at any point of time."
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
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,         CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,      CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
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
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,    CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
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
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_130 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_2_use_cases),
      cvd_devcfg_bw_level_2_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_45 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_bw_level_3_use_cases),
      cvd_devcfg_bw_level_3_use_cases
   },
   {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( CVD_DEVCFG_BW_MBPS_70 << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
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


static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[ ] =
  {
    /* 	First classification is based on EVS media id.		

	Use case of NB with DM_FV5_BROADSIDE
        1 voice session -> EVS-NB stream ->NB DM_FV5_BROADSIDE. RTD: 98.54ms
        1 voice session -> EVS-NB stream ->NB SM_ECNS. RTD: 88.27ms
        1 voice session -> EVS-NB stream ->NB SM_ECNS_V2. RTD: 87.64ms
	1 voice session -> EVS-NB/EVS-WB Stream -> DM_FV5 VocProc. RTD: 101.14 ms (EVS-NB), 106.47ms (EVS-WB)
        Allocate CVD_DEVCFG_MINSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }, 
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[ ] = 
  {
	/*	Only EVS media stream use cases which needs SVS2 are part of this classification
	*/
	/*  Use cases covered for each of the below topologies:
        1 voice session -> EVS-WB Stream -> SM_ECNS VocProc. RTD:  82.7ms (EVS-WB) 
        1 voice session -> EVS-WB Stream -> SM_ECNS_V2 VocProc. RTD: 86.08ms (EVS-WB). Power Dashboard use case. Strict RTD 87ms. 
	1 voice session -> EVS-NB/WB Stream -> QM_FP_V2 VocProc. RTD: 89.29ms (EVS-NB), 88.02ms (EVS-WB)
	1 voice session -> EVS-SWB/EVS-FB Stream -> DM_FV5 VocProc. RTD: 110.99ms (EVS-SWB), 118.89 ms(EVS-FB)
        1 voice session -> EVS-WB/EVS-SWB/EVS-FB Stream -> DM_FV5_BROADSIDE VocProc. RTD: 95.40ms (EVS-WB), 111.53ms (EVS-SWB), 119.47 ms (EVS-FB)
        1 voice session -> EVS-NB Stream -> VPM_TX_SM_FLUENCEV5 VocProc. RTD: 88.51 
		Allocate CVD_DEVCFG_SVS2
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }, 
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },    
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }, 
    { 1, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }, 
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA } 
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[ ] =
  {
    /*  First classification is based on EVS media.
	Use cases covered for each of the below topologies:
        1 voice session -> EVS-SWB/FB Stream ->  QM_FP_V2 VocProc. RTD: 103.04ms (EVS-SWB), 109.6 ms (EVS-FB)
        1 voice session -> EVS-WB Stream -> VPM_TX_SM_FLUENCEV5 VocProc. RTD:  98.48ms (EVS-WB) 
        Allocate CVD_DEVCFG_SVS
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2,CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA } 
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:        
        1 voice session -> EVS-SWB Stream -> (SWB) SM_ECNS_V2 VocProc. RTD: 88.99ms. Power Dashboard use case. Strict RTD 90ms.
        1 voice session -> EVS-NB/EVS-WB Stream -> DM_FV5 VocProc. RTD: 101.14 ms (EVS-NB), 106.47ms (EVS-WB)
	Allocate CVD_DEVCFG_SVS_PLUS
    */
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }  
  };


static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[ ] =
{
   /*Use cases covered for each of the below topologies
      1 voice session -> EVS-FB Stream -> (FB) SM_ECNS_v2 VocProc. RTD: 90.4ms. Power Dashboard use case. Strict RTD 90ms.
      1 voice session -> EVS-EVS-SWB/EVS-FB Stream -> VPM_TX_SM_FLUENCEV5 VocProc. RTD: 115.73ms (EVS-SWB), 123.07 ms (EVS-FB) 
      1 voice session -> EVS-SWB/EVS-FB Stream -> DM_FV5 VocProc. RTD: 110.99ms (EVS-SWB), 118.89 ms(EVS-FB)
      Allocate CVD_DEVCFG_NOMINAL
   */ 
   { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }, 
   { 1, 0, 0, 0, 1, 0, 0, 0, 1, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
   { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }  
};

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_5_use_cases[ ] =  
{	
   /*   Use case of NB with SM_ECNS and SM_ECNS_V2
        1 voice session -> NB stream -> NB SM_ECNS. RTD: 76.94ms. Power Dashboard use case. Strict RTD 80ms.
        1 voice session -> NB stream -> NB SM_ECNS_V2. RTD: 76.31ms. Power Dashboard use case. Strict RTD 80ms.
        1 voice session -> NB stream -> NB DM_VPECNS. RTD: 79.60ms. Power Dashboard use case. Strict RTD 80ms.
        Allocate CVD_DEVCFG_MINSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },  
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA } 
};

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_6_use_cases[ ] =
  {
    /*	Only non-EVS NB media stream and NB VocProc use cases which needs SVS2 are part of this classification
	*/
	/*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB SM_FV5 VocProc. RTD: 80.96ms     
		1 voice session -> NB Stream -> NB SM_FLUENCEV5 VocProc. RTD: 82.17ms
		1 voice session -> NB Stream -> NB DM_FV5 VocProc. RTD: 79.97ms
		1 voice session -> NB Stream -> NB DM_FV5_BROADSIDE VocProc. RTD: 82.7ms
		1 voice session -> NB Stream -> NB QM_FV5 VocProc. RTD: 81.96ms
		Allocate CVD_DEVCFG_SVS2
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },   
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },  
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }, 
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA } 
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_7_use_cases[ ] =
  {
    /*	Only non-EVS WB media stream and WB VocProc use cases which needs SVS2 are part of this classification
	*/
	/*  Use cases covered for each of the below topologies:
            1 voice session -> WB Stream -> WB SM_ECNS VocProc. RTD: 80.04ms
	    1 voice session -> WB Stream -> WB SM_ECNS_V2 VocProc. RTD: 79.90ms
            1 voice session -> WB Stream -> WB DM_VPECNS VocProc. RTD: 82.32ms
            1 voice session -> WB Stream -> WB QM_FLUENCE_PROV2 VocProc. RTD: 93.28ms
		
        Allocate CVD_DEVCFG_SVS2
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }, 
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }, 
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_8_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> WB(WVE) VocProc.
	Allocate CVD_DEVCFG_SVS2
    */
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }, 
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA } 
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_9_use_cases[ ] =
  {
	/*  Next classification is based on DM topology for non-EVS media.
	Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream ->  (WB) DM_FV5 VocProc. RTD: 79.85ms (NB), 82.74ms (WB)
        1 voice session -> NB/WB Stream -> (WB) DM_FV5_BROADSIDE VocProc. RTD: 78ms (NB), 81.13 (WB) 
        Allocate CVD_DEVCFG_SVS
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }, 
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }   
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_10_use_cases[ ] =
  {
     /*  Use cases covered for each of the below topologies:
         1 voice session -> NB/WB Stream -> WB SM_FV5 VocProc. RTD: 89ms
         Allocate CVD_DEVCFG_SVS2
       */
     { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA } 
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_11_use_cases[ ] =
{
   /*  Use cases covered for each of the below topologies:
         1 voice session -> Any Stream ->  TX_NONE VocProc.
         Allocate CVD_DEVCFG_MINSVS
       */
   { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 0, VPM_TX_NONE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
};

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_12_use_cases[ ] =
  {
    /*	All other use cases 
	Allocate CVD_DEVCFG_NOMINAL
	*/
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA } 
  };
  
static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[  ] =
  {
    { CVD_DEVCFG_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases), cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_SVS_PLUS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases), cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_NOMINAL, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_4_use_cases), cvd_devcfg_clock_level_4_use_cases },
    { CVD_DEVCFG_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_5_use_cases), cvd_devcfg_clock_level_5_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_6_use_cases), cvd_devcfg_clock_level_6_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_7_use_cases), cvd_devcfg_clock_level_7_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_8_use_cases), cvd_devcfg_clock_level_8_use_cases },
    { CVD_DEVCFG_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_9_use_cases), cvd_devcfg_clock_level_9_use_cases },
    { CVD_DEVCFG_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_10_use_cases), cvd_devcfg_clock_level_10_use_cases },
    { CVD_DEVCFG_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_11_use_cases), cvd_devcfg_clock_level_11_use_cases },
    { CVD_DEVCFG_NOMINAL, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_12_use_cases), cvd_devcfg_clock_level_12_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_MINSVS}, {CVD_DEVCFG_SVS2}, {CVD_DEVCFG_SVS}, {CVD_DEVCFG_SVS_PLUS}, {CVD_DEVCFG_NOMINAL}, {CVD_DEVCFG_NOMINAL_PLUS}, {CVD_DEVCFG_TURBO} } ;

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