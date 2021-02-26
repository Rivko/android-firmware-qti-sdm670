/*
  Copyright (C) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_devcfg/msm8916/cvd_devcfg.c#2 $
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

  1. Below table shows the Voice use case as media ID and Tx topology combination with floor_clock/RTD in ms:
     Topology/Media  |AMR-NB ,EFR    ,FR     ,HR     ,13K    ,EVRC   ,4GV-NB ,4GV-WB ,AMR-WB ,eAMR   ,EVS-NB ,EVS-WB ,EVS-SWB,EVS-FB ,
     _________________________________________________________________________________________________________________________________
     SM_ECNS         |SVS/67 ,SVS/67 ,SVS/66 ,SVS/67 ,SVS/67 ,SVS/67 ,SVS/70 ,SVS/75 ,SVS/72 ,SVS/75 ,SVS/77 ,SVS/81 ,SVS/76 ,SVS/81 ,
     DM_VPECNS       |SVS/68 ,SVS/68 ,SVS/67 ,SVS/68 ,SVS/68 ,SVS/68 ,SVS/71 ,SVS/76 ,SVS/73 ,SVS/76 ,SVS/78 ,SVS/82 ,SVS/81 ,SVS/86 ,
     SM_FLUENCEV5    |SVS/77 ,SVS/77 ,SVS/76 ,SVS/77 ,SVS/77 ,SVS/77 ,SVS/80 ,SVS/91 ,SVS/88 ,SVS/90 ,SVS/87 ,NOM/88 ,TUR/100,TUR/104,
     SM_ECNS_V2      |SVS/66 ,SVS/66 ,SVS/65 ,SVS/66 ,SVS/66 ,SVS/66 ,SVS/68 ,SVS/78 ,SVS/75 ,SVS/78 ,SVS/76 ,SVS/84 ,TUR/88 ,TUR/93 , 
     DM_F_V5_RED     |SVS/76 ,SVS/76 ,SVS/75 ,SVS/76 ,SVS/76 ,SVS/75 ,SVS/78 ,NOM/78 ,NOM/81 ,NOM/78 ,SVS/86 ,SVS/90 ,SVS/109,SVS/117, 
     DM_F_V5_BRO_RED |SVS/74 ,SVS/74 ,SVS/73 ,SVS/74 ,SVS/74 ,SVS/74 ,SVS/77 ,NOM/80 ,NOM/78 ,NOM/80 ,SVS/84 ,SVS/94 ,SVS/110,SVS/117,
     QM_F_PRO_RED    |SVS/78 ,SVS/77 ,SVS/76 ,SVS/78 ,SVS/77 ,SVS/77 ,SVS/80 ,SVS/92 ,SVS/89 ,SVS/91 ,SVS/87 ,SVS/98 ,SVS/115,NOM/108,

     Power Dash Board Use case: as media ID and Tx topology combination with floor_clock/RTD in ms
     Topology/Media  |AMR-NB ,AMR-WB , EVS-NB ,EVS-WB ,EVS-SWB,EVS-FB ,
     __________________________________________________________________
     SM_ECNS_V2      |SVS/66 ,SVS/75 , SVS/76 ,SVS/84 ,TUR/88 ,TUR/93 , 
     DM_F_V5_RED     |SVS/76 ,NOM/81 ,

     Strict RTD Use case: as media ID and Tx topology combination with floor_clock/RTD in ms
     Topology/Media  |AMR-NB ,AMR-WB ,EVS-NB ,EVS-WB ,EVS-SWB,EVS-FB ,
     __________________________________________________________________
     SM_ECNS_V2      |SVS/66 ,SVS/75 ,SVS/76 ,SVS/84 ,TUR/88 ,TUR/93 , 
     DM_F_V5_RED     |SVS/76 ,NOM/81 ,SVS/86 ,SVS/90 ,SVS/109,SVS/117, 
     Max Limit RTD   |   /90 ,   /90 ,   /90 ,   /90 ,   /127,   /127,
     
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
     | SVS         | 384.00    |
     ---------------------------
     | NOM         | 576.00    |
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
#define CVD_DEVCFG_CLK_HZ_SVS   ( 384000000 )
#define CVD_DEVCFG_CLK_HZ_NOM   ( 576000000 )
#define CVD_DEVCFG_CLK_HZ_TURBO ( 691200000 )

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
        1 voice session -> NB Stream -> NB VocProc.
        Clock: CVD_DEVCFG_CLK_HZ_SVS
    */
    { 1, CVD_UINT16_NA, 0, 0, 0, CVD_UINT16_NA, 0, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
};
static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> WB Stream -> WB VocProc with Tx topology: as SM_F_V5.
        1 voice session -> FB Stream -> FB VocProc with Tx topology: as QM_F_PRO.
        Clock: CVD_DEVCFG_CLK_HZ_NOM.
    */
    { 1, 0, CVD_UINT16_NA, 0,             0, 0, CVD_UINT16_NA, 0,              0, VPM_TX_SM_FLUENCEV5    , CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, 0,             0, 0, CVD_UINT16_NA, 0,              0, 0, CVD_UINT16_NA, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type and SM ECNS V2 Topology to meet RTD limit of 90:
        1 voice session -> SWB Stream -> NB/WB/SWB/FBP VocProc
        1 voice session -> FB Stream  -> NB/WB/SWB/FBP VocProc 
        Clock: CVD_DEVCFG_CLK_HZ_TURBO.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_ECNS_V2,   CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, VPM_TX_SM_FLUENCEV5, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[] =
  {
    /*  Use cases covered for EVS Media Type:
        1 voice session -> NB/WB/SWB/FBP Stream -> NB/WB/SWB/FBP VocProc.
        Clock: CVD_DEVCFG_CLK_HZ_SVS.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, VSS_MEDIA_ID_EVS, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_4_use_cases[] =
  {    
    /*  All NB Stream -> NB VocProc use cases. 
        Clock: CVD_DEVCFG_CLK_HZ_SVS.
    */
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream -> NB/WB VocProc.
        Clock: CVD_DEVCFG_CLK_HZ_SVS.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_NONE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS,          CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_ECNS_V2,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_QM_FLUENCE_PROV2, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_5_use_cases[] =
  {
    /*  Use cases covered for each of the below topologies:
        1 voice session -> NB/WB Stream -> NB/WB VocProc 
        Clock: CVD_DEVCFG_CLK_HZ_NOM.
    */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCE,             CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5,           CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, VPM_TX_DM_FLUENCEV5_BROADSIDE, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_6_use_cases[] =
  {
    /* All other use cases 
       Clock: CVD_DEVCFG_CLK_HZ_TURBO.
    */
    { CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT16_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[] =
  {
    { CVD_DEVCFG_CLK_HZ_SVS,   CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_0_use_cases), cvd_devcfg_clock_level_0_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,   CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_1_use_cases), cvd_devcfg_clock_level_1_use_cases },
    { CVD_DEVCFG_CLK_HZ_TURBO, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_2_use_cases), cvd_devcfg_clock_level_2_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS,   CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_3_use_cases), cvd_devcfg_clock_level_3_use_cases },
    { CVD_DEVCFG_CLK_HZ_SVS,   CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_4_use_cases), cvd_devcfg_clock_level_4_use_cases },
    { CVD_DEVCFG_CLK_HZ_NOM,   CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_5_use_cases), cvd_devcfg_clock_level_5_use_cases },
    { CVD_DEVCFG_CLK_HZ_TURBO, CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_level_6_use_cases), cvd_devcfg_clock_level_6_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { CVD_DEVCFG_SIZEOF_ARRAY(cvd_devcfg_clock_levels), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[ ] = { {CVD_DEVCFG_CLK_HZ_SVS}, {CVD_DEVCFG_CLK_HZ_NOM}, {CVD_DEVCFG_CLK_HZ_TURBO} } ;

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
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, VPM_TX_DM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[] =
  {
   /*  Use cases covered for each of the below topologies:
       1 voice session -> NB Stream -> WB(WVE) VocProc
       1 voice session -> WB Stream -> WB VocProc 
   */
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_SM_FLUENCEV5,     CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_FLUENCE,       CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA },
    { 1, CVD_UINT16_NA, CVD_UINT16_NA, 0, 0, 0, 1, 0, 0, VPM_TX_DM_VPECNS,        CVD_UINT32_NA, CVD_UINT32_NA, CVD_UINT16_NA }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_4_use_cases[] =
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

/* Undefine macros local to this file. */
#undef CVD_UINT16_NA 
#undef CVD_UINT32_NA 

