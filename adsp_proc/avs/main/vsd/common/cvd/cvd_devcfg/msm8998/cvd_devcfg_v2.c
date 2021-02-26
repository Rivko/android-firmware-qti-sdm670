/*
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/msm8998/cvd_devcfg_v2.c#2 $
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
	evs-nb					svs2		NA		svs2		minSVS		NA	    NA
	evs-wb					svs2		NA		svs2		svs2		NA	    NA
	evs-swb					nom			NA		svs2		svs2		NA	    NA
	evs-fb					nom			NA		NA			NA			svs		svs
	nb						minSVS		minSVS	svs2		svs2		NA	    NA
	wb						svs2		svs2	svs			svs			NA	    NA

	general use cases (this includes power dashboard use cases as well):
	
    media_id\topology_id 	sm_ecns_v2	sm_ecns	sm_fv5	dm_vpecns	dm_fv5_r	dm_fv5_b_r	dm_fv5	dm_fv5_b	qm_fp_v2  qm_fp_v2_r
	evs-nb					svs2		NA		svs2	NA			svs2		minSVS		svs2	minSVS		svs2	  svs2
	evs-wb					svs2		NA		nom		NA			svs2		svs2		svs2	svs2		svs2	  svs2
	evs-swb					nom			NA		nom		NA			svs2		svs2		svs2	svs2		svs		  svs
	evs-fb					nom			NA		nom		NA			svs2		svs2		svs		svs			nom		  svs
	nb						minSVS		minSVS	svs2	svs2		svs2		svs2		svs2	svs2		svs2	  svs2
	wb						svs2		svs2	svs2	svs2		svs			svs			svs		svs			svs2	  svs2

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

#define CVD_DEVCFG_V2_MINSVS  (201600000)

#define CVD_DEVCFG_V2_SVS2    (364800000)

#define CVD_DEVCFG_V2_SVS     (499200000)

#define CVD_DEVCFG_V2_NOMINAL (806400000)

/* Macros local to this file */
#define CVD_UINT16_NA ( 0xFFFF )
#define CVD_UINT32_NA ( 0xFFFFFFFF )

/*
   All voice usecases are covered with the assumption that, one voice session ( MVM ) is
   associated with only one VocProc session and only one Stream session at any point of time."
*/
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_0_use_cases[ ] =
  {
    /* 	First classification is based on EVS media id.

        Use case of NB with DM_FV5_BROADSIDE
        1 voice session -> EVS-NB stream ->NB DM_FV5_BROADSIDE. RTD: 105.32ms
        Allocate CVD_DEVCFG_V2_MINSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_1_use_cases[ ] =
  {
    /*  Only EVS media stream use cases which needs SVS2 are part of this classification
    */
    /*  Use cases covered for each of the below topologies:
        1 voice session -> EVS-NB/EVS-WB Stream -> SM_ECNS_V2 VocProc. RTD: 77ms (EVS-NB), 86.1ms (EVS-WB). Power Dashboard use case. Strict RTD 87ms.
        1 voice session -> EVS-NB/EVS-WB Stream -> SM_ECNS VocProc. RTD: 78ms (EVS-NB), 82.66ms (EVS-WB)
        1 voice session -> EVS-NB/EVS-WB Stream -> QM_FP_V2 VocProc. RTD: 89ms (EVS-NB), 100ms (EVS-WB)
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> EVS-NB/EVS-WB/EVS-SWB/EVS-FB Stream -> DM_FV5 VocProc. RTD: 87.34 ms (EVS-NB), 113.93ms (EVS-WB), 111.22ms (EVS-SWB), 119.24 ms(EVS-FB)
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> EVS-WB/EVS-SWB/EVS-FB Stream -> DM_FV5_BROADSIDE VocProc. RTD: 95.53ms (EVS-WB), 111.77ms (EVS-SWB), 119.84 ms (EVS-FB)
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_2_use_cases[ ] =
  {
    /*  First classification is based on EVS media.
        Use cases covered for each of the below topologies:
        1 voice session -> EVS-SWB Stream -> (SWB) QM_FP_V2 VocProc. RTD: 106.26ms
        1 voice session -> EVS-FB Stream -> (FB) QM_FP_V2 VocProc. RTD: 113.58ms
        Allocate CVD_DEVCFG_V2_SVS
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2,CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_3_use_cases[ ] =
  {
    /*	Only EVS media stream needs Nominal
    */	
    /*  Use cases covered for each of the below topologies:
        1 voice session -> EVS-WB Stream -> SM_FV5 VocProc. RTD: 83.67ms.
        1 voice session -> EVS-SWB Stream -> SM_FV5 VocProc. RTD: 95.94ms.
        1 voice session -> EVS-FB Stream -> SM_FV5 VocProc. RTD: 100.12ms.
        Allocate CVD_DEVCFG_V2_NOMINAL
    */
    { 1, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, 0, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },

    /*  Use cases covered for each of the below topologies:        
        1 voice session -> EVS-SWB Stream -> (SWB) SM_ECNS_V2 VocProc. RTD: 86.4ms. Power Dashboard use case. Strict RTD 87ms.
        1 voice session -> EVS-FB Stream -> (FB) SM_ECNS_v2 VocProc. RTD: 89ms. Power Dashboard use case. Strict RTD 87ms.
        Allocate CVD_DEVCFG_V2_NOMINAL
    */
    { 1, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_4_use_cases[ ] =  
{
    /*  classification is based on SM topology for non-EVS media.

        Use case of NB with SM_ECNS and SM_ECNS_V2
        1 voice session -> NB stream -> NB SM_ECNS. RTD: 79.79ms. Power Dashboard use case. Strict RTD 80ms.
        1 voice session -> NB stream -> NB SM_ECNS_V2. RTD: 79.43ms. Power Dashboard use case. Strict RTD 80ms.
        Allocate CVD_DEVCFG_V2_MINSVS
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS,    CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_ECNS_V2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_5_use_cases[ ] =
  {
    /*  Only non-EVS NB media stream and NB VocProc use cases which needs SVS2 are part of this classification
    */
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> NB SM_FV5 VocProc. RTD: 80.96ms
        1 voice session -> NB Stream -> NB DM_VPECNS VocProc. RTD: 72ms
        1 voice session -> NB Stream -> NB DM_FLUENCE VocProc. RTD: 79ms
        1 voice session -> NB Stream -> NB DM_FV5 VocProc. RTD: 79.85ms
        1 voice session -> NB Stream -> NB DM_FV5_BROADSIDE VocProc. RTD: 78.13ms
        1 voice session -> NB Stream -> NB QM_FV5 VocProc. RTD: 81.89ms
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_SM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,              CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_QM_FLUENCE_PROV2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_6_use_cases[ ] =
  {
    /*	Only non-EVS WB media stream and WB VocProc use cases which needs SVS2 are part of this classification
    */
    /*  Use cases covered for each of the below topologies:
        1 voice session -> WB Stream -> WB SM_ECNS VocProc. RTD: 76ms
        1 voice session -> WB Stream -> WB SM_ECNS_V2 VocProc. RTD: 79.48ms
        1 voice session -> WB Stream -> WB SM_FV5 VocProc. RTD: 80.68ms
        1 voice session -> WB Stream -> WB DM_VPECNS VocProc. RTD: 77ms
        1 voice session -> WB Stream -> WB DM_FLUENCE VocProc. RTD: 79.4ms
        1 voice session -> WB Stream -> WB QM_FP_V2 VocProc. RTD: 93.37ms
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,      CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,    CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_7_use_cases[ ] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB Stream -> WB(WVE) VocProc.
        Allocate CVD_DEVCFG_V2_SVS2
    */
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_NONE,         CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS,      CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_ECNS_V2,   CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_8_use_cases[ ] =
  {
    /*  Next classification is based on DM topology for non-EVS media.
        Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream ->  (WB) DM_FV5 VocProc. RTD: 79.85ms (NB), 80.16ms (WB)
        1 voice session -> NB/WB Stream -> (WB) DM_FV5_BROADSIDE VocProc. RTD: 78ms (NB), 82.588 (WB)
        Allocate CVD_DEVCFG_V2_SVS
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };
  
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_v2_clock_level_9_use_cases[ ] =
  {
    /*  All other use cases 
        Allocate CVD_DEVCFG_V2_NOMINAL
    */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_v2_clock_levels[ ] =
  {
    { CVD_DEVCFG_V2_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_0_use_cases), cvd_devcfg_v2_clock_level_0_use_cases },
    { CVD_DEVCFG_V2_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_1_use_cases), cvd_devcfg_v2_clock_level_1_use_cases },
    { CVD_DEVCFG_V2_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_2_use_cases), cvd_devcfg_v2_clock_level_2_use_cases },
    { CVD_DEVCFG_V2_NOMINAL, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_3_use_cases), cvd_devcfg_v2_clock_level_3_use_cases },
    { CVD_DEVCFG_V2_MINSVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_4_use_cases), cvd_devcfg_v2_clock_level_4_use_cases },
    { CVD_DEVCFG_V2_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_5_use_cases), cvd_devcfg_v2_clock_level_5_use_cases },
    { CVD_DEVCFG_V2_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_6_use_cases), cvd_devcfg_v2_clock_level_6_use_cases },
    { CVD_DEVCFG_V2_SVS2, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_7_use_cases), cvd_devcfg_v2_clock_level_7_use_cases },
    { CVD_DEVCFG_V2_SVS, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_8_use_cases), cvd_devcfg_v2_clock_level_8_use_cases },
    { CVD_DEVCFG_V2_NOMINAL, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_level_9_use_cases), cvd_devcfg_v2_clock_level_9_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table_v2 =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_levels), cvd_devcfg_v2_clock_levels };
  
static cvd_devcfg_clock_frequency_level_t cvd_devcfg_v2_clock_frequency_levels[ ] = { {CVD_DEVCFG_V2_MINSVS}, {CVD_DEVCFG_V2_SVS2}, {CVD_DEVCFG_V2_SVS}, {CVD_DEVCFG_V2_NOMINAL} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table_v2 = {CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_v2_clock_frequency_levels), cvd_devcfg_v2_clock_frequency_levels } ; 

  
/* Undefine macros local to this file. */
#undef CVD_UINT16_NA
#undef CVD_UINT32_NA