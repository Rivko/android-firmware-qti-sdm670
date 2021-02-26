/*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_devcfg_parser.c#4 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#ifndef WINSIM
  #include "DALSys.h"
  #include "DALSysTypes.h"
  #include "DDIChipInfo.h"
  #include "DalDevice.h"
  #include "DALDeviceId.h"
  #include "DDIClock.h"
#endif /* !WINSIM */

#include "msg.h"
#include "err.h"
#include "apr_errcodes.h"
#include "qurt_elite.h"
#include "vss_private_if.h"
/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
#define DAL_PROP_NAME_SIZE 15

#define CVD_DEVCFG_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define CVD_DEVCFG_DEFAULT_NUM_PREEMPTION_SAFETY_MARGINS ( 5 )

#define CVD_DEVCFG_DEFAULT_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US ( 1000 )
  /**<
   * Additional 1ms safety margin in the VpTx and VpRx timing offsets that CVD
   * sets on VCP. This is needed to take care of thread preemption delay.
   */

#define CVD_DEVCFG_DEFAULT_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US ( 1500 )
  /**<
   * Additional 1.5ms safety margin in the Venc and Vdec timing offsets that CVD
   * sets on VCP. This is needed to take care of thread preemption delay.*/

#define CVD_DEVCFG_MAX_NUM_CLK_PERF_LEVELS ( 15 )
/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/
static cvd_devcfg_clock_table_t* cvd_devcfg_parser_clock_table = NULL;
static cvd_devcfg_preemption_safety_margins_table_t* cvd_devcfg_parser_preemption_safety_margins_table = NULL;
static cvd_devcfg_voice_use_case_na_values_t* cvd_devcfg_parser_voice_use_case_na_values = NULL;
static uint32_t cvd_devcfg_parser_cycles_per_thousand_instr_packet;
static uint32_t cvd_devcfg_parser_mmpm_sleep_latency_us;
static uint32_t cvd_devcfg_parser_max_q6_core_clock_mhz = 729; /* Set default in case parser failed. */
static uint32_t cvd_devcfg_parser_csm_sleep_threshold_us;
static uint32_t cvd_devcfg_parser_csm_safety_margin_us;
static cvd_devcfg_preemption_safety_margins_values_t default_preemption_safety_margin_table;  
static cvd_devcfg_clock_frequency_level_table_t* cvd_devcfg_parser_clock_frequency_table =  NULL ;
/* Set default Mininum core clock to 300 Mhz in case parser failed. */
static uint32_t cvd_devcfg_parser_min_q6_core_clock = 300;

#ifndef WINSIM
static uint32_t cvd_devcfg_parser_num_clk_perf_levels = 0;
static uint32_t cvd_devcfg_parser_clk_perf_levels_khz[ CVD_DEVCFG_MAX_NUM_CLK_PERF_LEVELS ];
static cvd_devcfg_mmpm_core_info_t* cvd_devcfg_parser_mmpm_core_info = NULL;
static cvd_devcfg_mmpm_bw_table_t* cvd_devcfg_parser_bw_table = NULL;
static uint32_t cvd_devcfg_parser_mmpm_cpp = 1; /* Set default to 1 in case parser failed. */
/* Set the default minimal BW incase parser failed. */
static uint64_t cvd_devcfg_parser_max_bw = ( 90 << 20 );/* 90 MBps. */

#endif /* !WINSIM */

/****************************************************************************
 * INTERNAL ROUTINES                                                        *
 ****************************************************************************/
#ifndef WINSIM

static uint32_t cvd_devcfg_extract_min_q6_core_clock( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  uint32_t clock_table_size = 0;
  uint32_t clock_level;

  if ( NULL == cvd_devcfg_parser_clock_table )
  {
    return APR_EFAILED;
  }

  clock_table_size = cvd_devcfg_parser_clock_table->num_clock_levels;

  /* Init with clock value at 0th index for comparison. */
  cvd_devcfg_parser_min_q6_core_clock = 
    cvd_devcfg_parser_clock_table->clock_levels[0].core_floor_clock_hz;

  /* Loop around all the clock values in the cvd_devcfg clock table and cache 
     the minimal q6 core clock. */
  for( index = 0; index < clock_table_size; index++ )
  {
    clock_level =
      cvd_devcfg_parser_clock_table->clock_levels[index].core_floor_clock_hz;
    if( clock_level < cvd_devcfg_parser_min_q6_core_clock )
    {
      cvd_devcfg_parser_min_q6_core_clock = clock_level;
    }
  }

  /* Convert to MHz. */
  cvd_devcfg_parser_min_q6_core_clock /= 1000000;

  return rc;
}

static uint32_t cvd_devcfg_extract_max_bw( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index = 0;
  uint32_t bw_table_size = 0;
  uint64_t bw;

  if ( NULL == cvd_devcfg_parser_bw_table )
  {
    return APR_EFAILED;
  }

  bw_table_size = cvd_devcfg_parser_bw_table->num_bw_values;

  /* Init with bw value at 0th index for comparison. */
  cvd_devcfg_parser_max_bw = 
    cvd_devcfg_parser_bw_table->bw_values[0].bw_val.bwValue.busBwValue.bwBytePerSec;

  /* Loop around all the bw values in the cvd_devcfg bw table and cache 
     the maximum bw. */
  for( index = 0; index < bw_table_size; index++ )
  {
    bw = cvd_devcfg_parser_bw_table->bw_values[index].bw_val.bwValue.busBwValue.bwBytePerSec;
    if( bw > cvd_devcfg_parser_max_bw )
    {
      cvd_devcfg_parser_max_bw = bw;
    }
  }

  return rc;
}

uint32_t cvd_devcfg_parser_get_clock_frequency_info( void )
{
  uint32_t clk_level ;
  cvd_devcfg_clock_frequency_level_t* clk_freq_level ;
  cvd_devcfg_parser_num_clk_perf_levels = cvd_devcfg_parser_clock_frequency_table->num_clock_frequency_levels;

  for( clk_level = 0;clk_level < cvd_devcfg_parser_num_clk_perf_levels ; clk_level++ )
  {
    clk_freq_level = &cvd_devcfg_parser_clock_frequency_table->clock_frequency_levels[clk_level] ;
    cvd_devcfg_parser_clk_perf_levels_khz[clk_level] = clk_freq_level->clock_freq_hz/1000 ;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_devcfg_parser_get_clock_frequency_info():"
           "clock levels %d,  %d", cvd_devcfg_parser_clk_perf_levels_khz[clk_level], cvd_devcfg_parser_num_clk_perf_levels );
  }
  cvd_devcfg_parser_max_q6_core_clock_mhz = cvd_devcfg_parser_clk_perf_levels_khz[cvd_devcfg_parser_num_clk_perf_levels - 1]/1000;

  return APR_EOK;
}

static uint32_t cvd_devcfg_parser_get_clock_info( void )
{
#ifndef SIM
  DalDeviceHandle   *hClock;
  DALResult         dal_rc;
  ClockIdType       nClockId;
  ClockFreqPlanType *aFreqPlan;
  uint32            nFreqPlanLen;
  boolean is_core_api_success = TRUE;
  
  for (;;)
  {
    /* Attach to clock driver. */
    dal_rc = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hClock);
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_info(): Failed to get DAL handle for " \
                                            "device ID %x.",
                                            DALDEVICEID_CLOCK );
      is_core_api_success = FALSE ;
      break;
    }

    /* Get CPU clock id for use in other APIs. */
    dal_rc = DalClock_GetClockId(hClock, "q6_core_clk", &nClockId);
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_info(): Failed to get clock ID for " \
                                            " %s.",
                                            "q6_core_clk" );
      is_core_api_success = FALSE ;
      break;
    }
  
    /* Call with NULL array first to get required length. */
    dal_rc = DalClock_GetFrequencyPlan(hClock, nClockId, NULL, &nFreqPlanLen);
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, " cvd_devcfg_parser_get_clock_info(): Failed to get number of " \
                                            "clock frequency levels for device ID %s.",
                                            DALDEVICEID_CLOCK );
      is_core_api_success = FALSE ;
      break;
    }

    /* Allocate frequency plan array. */
    aFreqPlan = ( ClockFreqPlanType* )qurt_elite_memory_malloc( nFreqPlanLen * sizeof(ClockFreqPlanType), QURT_ELITE_HEAP_DEFAULT );

    /* Call again with newly allocated array to populate it. */
    dal_rc = DalClock_GetFrequencyPlan(hClock, nClockId, &aFreqPlan, &nFreqPlanLen);
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_info(): Failed to get clock perf levels " \
                                            "device ID %s.",
                                            DALDEVICEID_CLOCK );
      is_core_api_success = FALSE ;
      break;
    }

    cvd_devcfg_parser_num_clk_perf_levels = nFreqPlanLen;
    for( int clk_level = 0;clk_level < nFreqPlanLen; clk_level++ )
    {
      cvd_devcfg_parser_clk_perf_levels_khz[clk_level] = aFreqPlan[clk_level].nFreqHz/1000;
    }
    cvd_devcfg_parser_max_q6_core_clock_mhz = cvd_devcfg_parser_clk_perf_levels_khz[cvd_devcfg_parser_num_clk_perf_levels - 1]/1000;
    qurt_elite_memory_free( aFreqPlan );
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_devcfg_parser_get_clock_info(): max clock " \
                                            "%d %d",
                                            cvd_devcfg_parser_max_q6_core_clock_mhz, cvd_devcfg_parser_num_clk_perf_levels );
    break;

  }
  if ( FALSE == is_core_api_success )
  {
    ( void ) cvd_devcfg_parser_get_clock_frequency_info() ; //clock frequency levels 
    
  }

#endif
  return APR_EOK;
}

#endif

/****************************************************************************
 * ROUTINES                                                                 *
 ****************************************************************************/

APR_INTERNAL int32_t cvd_devcfg_parser_init ( void )
{
#if defined( WINSIM )

  /* Not needed on the simulator. */
  return APR_EOK;

#else

  int32_t rc = APR_EOK;
  DALResult dal_rc;
  DALSYS_PROPERTY_HANDLE_DECLARE( dev_prop_handle );  
  DALSYSPropertyVar clk_tbl_prop;
  DALSYSPropertyVar clk_tbl_prop_v2;
  DALSYSPropertyVar preemption_safety_margins_tbl_prop;
  DALSYSPropertyVar use_case_na_val_prop;
  DALSYSPropertyVar mmpm_core_info_prop;
  DALSYSPropertyVar mmpm_bw_requirement_prop;
  DALSYSPropertyVar mmpm_sleep_latency_prop;
  DALSYSPropertyVar cycles_per_thousand_instr_packet_prop;
  DALSYSPropertyVar mmpm_cpp_prop;
  DALSYSPropertyVar max_q6_core_clock_mhz;
  DALSYSPropertyVar csm_sleep_threshold_us;
  DALSYSPropertyVar csm_safety_margin_us;
  DALSYSPropertyVar clk_freq_tbl_prop ;
  DalChipInfoVersionType chip_version;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_devcfg_parser_init()======" );
  
  for ( ;; )
  { /* Read all the CVD device configurations from the devcfg xml file. */
    dal_rc = DALSYS_GetDALPropertyHandleStr(
               CVD_DEVCFG_DEV_ID_NAME, dev_prop_handle );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property handle for " \
                                              "device ID %s.",
                                              CVD_DEVCFG_DEV_ID_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle, CVD_DEVCFG_PREEMPTION_SAFETY_MARGINS_TABLE_PROP_NAME, 0, 
               &preemption_safety_margins_tbl_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_PREEMPTION_SAFETY_MARGINS_TABLE_PROP_NAME );
      default_preemption_safety_margin_table.vptx_preemption_safety_margin = CVD_DEVCFG_DEFAULT_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US;
      default_preemption_safety_margin_table.vprx_preemption_safety_margin = CVD_DEVCFG_DEFAULT_MVM_VOCPROC_PREMPTION_SAFETY_MARGIN_US;
      default_preemption_safety_margin_table.venc_preemption_safety_margin = CVD_DEVCFG_DEFAULT_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US;
      default_preemption_safety_margin_table.vdec_preemption_safety_margin = CVD_DEVCFG_DEFAULT_MVM_STREAM_PREMPTION_SAFETY_MARGIN_US;      
    }
    else
    {
        cvd_devcfg_parser_preemption_safety_margins_table =
      ( ( cvd_devcfg_preemption_safety_margins_table_t* ) preemption_safety_margins_tbl_prop.Val.pStruct );
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle, CVD_DEVCFG_VOICE_USE_CASE_NA_VALUES_PROP_NAME, 0,
               &use_case_na_val_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_VOICE_USE_CASE_NA_VALUES_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle, CVD_DEVCFG_MMPM_CORE_INFO_PROP_NAME, 0,
               &mmpm_core_info_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_MMPM_CORE_INFO_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle, CVD_DEVCFG_MMPM_BW_REQUIREMENT_PROP_NAME, 0,
               &mmpm_bw_requirement_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_MMPM_BW_REQUIREMENT_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle, CVD_DEVCFG_MMPM_SLEEP_LATENCY_PROP_NAME, 0,
               &mmpm_sleep_latency_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_MMPM_SLEEP_LATENCY_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle,
               CVD_DEVCFG_CYCLES_PER_THOUSAND_INSTR_PACKET_PROP_NAME, 0,
               &cycles_per_thousand_instr_packet_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_CYCLES_PER_THOUSAND_INSTR_PACKET_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle,
               CVD_DEVCFG_MMPM_CYCLES_PER_INSTR_PACKET_PROP_NAME, 0,
               &mmpm_cpp_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_MMPM_CYCLES_PER_INSTR_PACKET_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle,
               CVD_DEVCFG_MAX_Q6_CORE_CLOCK_MHZ, 0,
               &max_q6_core_clock_mhz );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_MAX_Q6_CORE_CLOCK_MHZ );
      rc = APR_EFAILED;
      break;
    }
    
    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle,
               CVD_DEVCFG_CSM_SLEEP_THRESHOLD_US, 0,
               &csm_sleep_threshold_us );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_CSM_SLEEP_THRESHOLD_US );
      rc = APR_EFAILED;
      break;
    }

    dal_rc = DALSYS_GetPropertyValue(
               dev_prop_handle,
               CVD_DEVCFG_CSM_SAFETY_MARGIN_US, 0,
               &csm_safety_margin_us );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
                                              "property %s.",
                                              CVD_DEVCFG_CSM_SAFETY_MARGIN_US );
      rc = APR_EFAILED;
      break;
    }

#ifdef SIM
    chip_version = DALCHIPINFO_VERSION( 2,0 );
#else // !SIM
    chip_version = DalChipInfo_ChipVersion();
#endif // !SIM
    
    if ( chip_version >= DALCHIPINFO_VERSION( 2,0 ) )
    {
      dal_rc = DALSYS_GetPropertyValue( dev_prop_handle, CVD_DEVCFG_CLOCK_TABLE_V2_PROP_NAME, 0,&clk_tbl_prop_v2 );
      if ( dal_rc != DAL_SUCCESS )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
               "property %s.", CVD_DEVCFG_CLOCK_TABLE_V2_PROP_NAME );
        rc = APR_ENOTEXIST; /* Do not bail out here, the V2 table might not be defined so use the default table */
      }
      else
      {
        cvd_devcfg_parser_clock_table = ( ( cvd_devcfg_clock_table_t* ) clk_tbl_prop_v2.Val.pStruct );
      }
    }

    if( APR_ENOTEXIST == rc || chip_version < DALCHIPINFO_VERSION( 2,0 ) )
    {
      dal_rc = DALSYS_GetPropertyValue(dev_prop_handle, CVD_DEVCFG_CLOCK_TABLE_PROP_NAME, 0,&clk_tbl_prop );
      if ( dal_rc != DAL_SUCCESS )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
               "property %s.", CVD_DEVCFG_CLOCK_TABLE_PROP_NAME );
        rc = APR_EFAILED;
        break;
      }
      rc = APR_EOK;
      cvd_devcfg_parser_clock_table = ( ( cvd_devcfg_clock_table_t* ) clk_tbl_prop.Val.pStruct );
    }

    dal_rc = DALSYS_GetPropertyValue( dev_prop_handle, CVD_DEVCFG_CLOCK_FREQ_TABLE_PROP_NAME, 0,&clk_freq_tbl_prop );
    if ( dal_rc != DAL_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_init(): Failed to get property value for " \
             "property %s.", CVD_DEVCFG_CLOCK_FREQ_TABLE_PROP_NAME );
      rc = APR_EFAILED;
      break;
    }

    cvd_devcfg_parser_clock_frequency_table = ( ( cvd_devcfg_clock_frequency_level_table_t* ) clk_freq_tbl_prop.Val.pStruct );

    cvd_devcfg_parser_voice_use_case_na_values =
      ( ( cvd_devcfg_voice_use_case_na_values_t* ) use_case_na_val_prop.Val.pStruct );

    cvd_devcfg_parser_mmpm_core_info =
      ( ( cvd_devcfg_mmpm_core_info_t* ) mmpm_core_info_prop.Val.pStruct );

    cvd_devcfg_parser_bw_table =
      ( ( cvd_devcfg_mmpm_bw_table_t* ) mmpm_bw_requirement_prop.Val.pStruct );

    cvd_devcfg_parser_mmpm_sleep_latency_us = mmpm_sleep_latency_prop.Val.dwVal;

    cvd_devcfg_parser_cycles_per_thousand_instr_packet =
      cycles_per_thousand_instr_packet_prop.Val.dwVal;

    cvd_devcfg_parser_mmpm_cpp =
      mmpm_cpp_prop.Val.dwVal;

    cvd_devcfg_parser_max_q6_core_clock_mhz =
      max_q6_core_clock_mhz.Val.dwVal;

    cvd_devcfg_parser_csm_sleep_threshold_us =
      csm_sleep_threshold_us.Val.dwVal;

    cvd_devcfg_parser_csm_safety_margin_us = 
       csm_safety_margin_us.Val.dwVal;
    
    ( void ) cvd_devcfg_extract_min_q6_core_clock();
    ( void ) cvd_devcfg_extract_max_bw();
	( void ) cvd_devcfg_parser_get_clock_info();

    break;
  }
  /* Panic when xml file read has failed. */
  CVD_DEVCFG_PANIC_ON_ERROR( rc );

  return rc;

#endif /* WINSIM */

}

APR_INTERNAL int32_t cvd_devcfg_parser_deinit ( void )
{
  return APR_EOK;
}

#ifndef WINSIM

APR_INTERNAL int32_t cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config (
  cvd_devcfg_parser_voice_use_case_t* use_case,
  cvd_devcfg_parser_target_specific_config_t* ret_clock_and_bus_preempt_margin_config
)
{
  uint32_t clk_level_idx;
  uint32_t bw_level_idx;
  uint32_t use_case_idx;
  uint32_t preempt_margin_level_idx;
  uint32_t num_clk_levels;
  uint32_t num_bw_levels;
  uint32_t num_preemption_margin_levels;
  uint32_t num_supported_use_cases;
  bool_t clock_level_use_case_found = FALSE;
  bool_t bw_level_use_case_found = FALSE;
  bool_t premption_level_use_case_found = FALSE;
  cvd_devcfg_clock_level_t* clk_level;
  cvd_devcfg_mmpm_bw_value_t* bw_level;
  cvd_devcfg_preemption_safety_margins_level_t* preemption_margin_level;
  cvd_devcfg_supported_voice_use_case_t* supported_use_case;
  cvd_devcfg_voice_use_case_na_values_t* na_val;

  if ( ( use_case == NULL ) || ( ret_clock_and_bus_preempt_margin_config == NULL ) )
  {
     return APR_EBADPARAM;
  }

  if ( ( cvd_devcfg_parser_clock_table == NULL ) || ( cvd_devcfg_parser_voice_use_case_na_values == NULL ) )
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): " \
                                           "Invalid input table" );
     return APR_EBADPARAM;
  }

  if ( cvd_devcfg_parser_preemption_safety_margins_table == NULL )
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): " \
                                           "Invalid input cvd_devcfg_parser_preemption_safety_margins_table" );   
  }

#ifndef WINSIM
  if ( ( cvd_devcfg_parser_mmpm_core_info == NULL ) || ( cvd_devcfg_parser_bw_table == NULL ) )
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): " \
                                           "Invalid mmpm info" );
     return APR_EBADPARAM;
  }
#endif /* !WINSIM */

  num_clk_levels = cvd_devcfg_parser_clock_table->num_clock_levels;
  na_val = cvd_devcfg_parser_voice_use_case_na_values;

  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_devcfg_parser: voice_sessions: %d, nb_streams: %d, wb_streams: %d" \
                                        "swb_streams: %d, fb_plus_streams: %d, nb_vocprocs: %d",
                                        use_case->num_voice_sessions, use_case->num_nb_streams,
                                        use_case->num_wb_streams, use_case->num_swb_streams,
                                        use_case->num_fb_plus_streams, use_case->num_nb_vocprocs );

  MSG_7( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_devcfg_parser: wb_vocprocs: %d, swb_vocprocs: %d, fb_plus_vocprocs: %d" \
                                        "tx_topology_id: 0x%08X, rx_topology_id: 0x%08X, media_id: 0x%04X, " \
                                        "vfr_mode: 0x%08X", use_case->num_wb_vocprocs, use_case->num_swb_vocprocs,
                                        use_case->num_fb_plus_vocprocs, use_case->tx_topology_id,
                                        use_case->rx_topology_id, use_case->media_id, use_case->vfr_mode );

  /* Find the first clock level in supported use-case table in cvd_devcfg.c that
   * can support the client specified voice use case.
   */
  for ( clk_level_idx = 0; clk_level_idx < num_clk_levels; ++clk_level_idx )
  {
     clk_level = &cvd_devcfg_parser_clock_table->clock_levels[ clk_level_idx ];
     num_supported_use_cases = clk_level->num_supported_use_cases;

     for ( use_case_idx = 0; use_case_idx < num_supported_use_cases; ++use_case_idx )
     {
       supported_use_case = &clk_level->supported_use_cases[ use_case_idx ];

       if ( 
           ( ( supported_use_case->max_num_voice_sessions ==
               na_val->max_num_voice_sessions ) ||
             ( use_case->num_voice_sessions <=
               supported_use_case->max_num_voice_sessions ) ) &&

           ( ( supported_use_case->max_num_nb_streams ==
               na_val->max_num_nb_streams ) ||
             ( use_case->num_nb_streams <=
               supported_use_case->max_num_nb_streams ) ) &&

           ( ( supported_use_case->max_num_wb_streams ==
               na_val->max_num_wb_streams ) ||
             ( use_case->num_wb_streams <=
               supported_use_case->max_num_wb_streams ) ) &&

          ( ( supported_use_case->max_num_swb_streams ==
              na_val->max_num_swb_streams ) ||
            ( use_case->num_swb_streams <=
              supported_use_case->max_num_swb_streams ) ) &&

          ( ( supported_use_case->max_num_fb_plus_streams ==
              na_val->max_num_fb_plus_streams ) ||
            ( use_case->num_fb_plus_streams <=
              supported_use_case->max_num_fb_plus_streams ) ) &&

           ( ( supported_use_case->max_num_nb_vocprocs ==
               na_val->max_num_nb_vocprocs ) ||
             ( use_case->num_nb_vocprocs <=
               supported_use_case->max_num_nb_vocprocs ) ) &&

           ( ( supported_use_case->max_num_wb_vocprocs ==
               na_val->max_num_wb_vocprocs ) ||
             ( use_case->num_wb_vocprocs <=
               supported_use_case->max_num_wb_vocprocs ) ) &&

          ( ( supported_use_case->max_num_swb_vocprocs ==
              na_val->max_num_swb_vocprocs ) ||
            ( use_case->num_swb_vocprocs <=
              supported_use_case->max_num_swb_vocprocs ) ) &&

          ( ( supported_use_case->max_num_fb_plus_vocprocs ==
              na_val->max_num_fb_plus_vocprocs ) ||
            ( use_case->num_fb_plus_vocprocs <=
              supported_use_case->max_num_fb_plus_vocprocs ) ) &&

           ( ( supported_use_case->tx_topology_id ==
               na_val->tx_topology_id ) ||
             ( use_case->tx_topology_id ==
               supported_use_case->tx_topology_id ) ) &&

           ( ( supported_use_case->rx_topology_id ==
               na_val->rx_topology_id ) ||
             ( use_case->rx_topology_id ==
               supported_use_case->rx_topology_id ) ) &&

          ( ( supported_use_case->media_id ==
              na_val->media_id ) ||
            ( use_case->media_id ==
              supported_use_case->media_id ) ) &&

           ( ( supported_use_case->vfr_mode == na_val->vfr_mode ) ||
             ( use_case->vfr_mode == supported_use_case->vfr_mode ) )
         )
       {
         ret_clock_and_bus_preempt_margin_config->core_floor_clock_hz = clk_level->core_floor_clock_hz;

         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "cvd_devcfg_parser: core_clock_hz %d ",
                ret_clock_and_bus_preempt_margin_config->core_floor_clock_hz );
         clock_level_use_case_found = TRUE;
         break;
       }
     }

    if( clock_level_use_case_found == TRUE )
    {
      break;
    }
  }

  ret_clock_and_bus_preempt_margin_config->cycles_per_thousand_instr_packets =
    cvd_devcfg_parser_cycles_per_thousand_instr_packet;

  ret_clock_and_bus_preempt_margin_config->sleep_latency_us = cvd_devcfg_parser_mmpm_sleep_latency_us;
  
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "cvd_devcfg_parser: cycles_per_thousand_instr_packets %d, sleep_latency_us %d",
         ret_clock_and_bus_preempt_margin_config->cycles_per_thousand_instr_packets,
         ret_clock_and_bus_preempt_margin_config->sleep_latency_us );

  if ( cvd_devcfg_parser_preemption_safety_margins_table == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_devcfg_parser: " \
                                         "cvd_devcfg_parser_preemption_safety_margins_table pointer is NULL, hence default values set" );

    ret_clock_and_bus_preempt_margin_config->vptx_preemption_safety_margin = default_preemption_safety_margin_table.vptx_preemption_safety_margin;

    ret_clock_and_bus_preempt_margin_config->vprx_preemption_safety_margin = default_preemption_safety_margin_table.vprx_preemption_safety_margin;

    ret_clock_and_bus_preempt_margin_config->venc_preemption_safety_margin = default_preemption_safety_margin_table.venc_preemption_safety_margin;

    ret_clock_and_bus_preempt_margin_config->vdec_preemption_safety_margin = default_preemption_safety_margin_table.vdec_preemption_safety_margin;

    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_devcfg_parser: safety margins - vptx: %d, vprx: %d, venc: %d, vdec: %d",
                                           ret_clock_and_bus_preempt_margin_config->vptx_preemption_safety_margin,
                                           ret_clock_and_bus_preempt_margin_config->vprx_preemption_safety_margin,
                                           ret_clock_and_bus_preempt_margin_config->venc_preemption_safety_margin,
                                           ret_clock_and_bus_preempt_margin_config->vdec_preemption_safety_margin);

    premption_level_use_case_found = TRUE;        
  }
  else
  /* Find the appropriate preemption margins in cvd_devcfg.c that can support the client specified voice use case.
   */
  {
    num_preemption_margin_levels = cvd_devcfg_parser_preemption_safety_margins_table->num_preemption_margin_levels;

    for ( preempt_margin_level_idx = 0; preempt_margin_level_idx < num_preemption_margin_levels; ++preempt_margin_level_idx )
    {
      preemption_margin_level = &cvd_devcfg_parser_preemption_safety_margins_table->preemption_margin_levels[ preempt_margin_level_idx ];
      num_supported_use_cases = preemption_margin_level->num_supported_use_cases;

      for ( use_case_idx = 0; use_case_idx < num_supported_use_cases; ++use_case_idx )
      {
        supported_use_case = &preemption_margin_level->supported_use_cases[ use_case_idx ];

        if ( ( ( supported_use_case->max_num_voice_sessions == 
                na_val->max_num_voice_sessions ) ||
               ( use_case->num_voice_sessions <= 
                supported_use_case->max_num_voice_sessions ) ) &&

               ( ( supported_use_case->max_num_nb_streams == 
                na_val->max_num_nb_streams ) ||
               ( use_case->num_nb_streams <= 
                supported_use_case->max_num_nb_streams ) ) &&

               ( ( supported_use_case->max_num_wb_streams == 
                na_val->max_num_wb_streams ) ||
               ( use_case->num_wb_streams <= 
                supported_use_case->max_num_wb_streams ) ) &&

               ( ( supported_use_case->max_num_swb_streams == 
               na_val->max_num_swb_streams ) ||
               ( use_case->num_swb_streams <= 
               supported_use_case->max_num_swb_streams ) ) &&

               ( ( supported_use_case->max_num_fb_plus_streams == 
               na_val->max_num_fb_plus_streams ) ||
               ( use_case->num_fb_plus_streams <= 
               supported_use_case->max_num_fb_plus_streams ) ) &&

               ( ( supported_use_case->max_num_nb_vocprocs == 
               na_val->max_num_nb_vocprocs ) ||
               ( use_case->num_nb_vocprocs <= 
               supported_use_case->max_num_nb_vocprocs ) ) &&

               ( ( supported_use_case->max_num_wb_vocprocs == 
               na_val->max_num_wb_vocprocs ) ||
               ( use_case->num_wb_vocprocs <= 
               supported_use_case->max_num_wb_vocprocs ) ) &&

               ( ( supported_use_case->max_num_swb_vocprocs == 
               na_val->max_num_swb_vocprocs ) ||
               ( use_case->num_swb_vocprocs <= 
               supported_use_case->max_num_swb_vocprocs ) ) &&

               ( ( supported_use_case->max_num_fb_plus_vocprocs == 
               na_val->max_num_fb_plus_vocprocs ) ||
               ( use_case->num_fb_plus_vocprocs <= 
               supported_use_case->max_num_fb_plus_vocprocs ) ) &&

               ( ( supported_use_case->tx_topology_id == 
               na_val->tx_topology_id ) ||
               ( use_case->tx_topology_id == 
               supported_use_case->tx_topology_id ) ) &&

               ( ( supported_use_case->rx_topology_id == 
               na_val->rx_topology_id ) ||
               ( use_case->rx_topology_id == 
               supported_use_case->rx_topology_id ) ) &&

               ( ( supported_use_case->media_id == 
               na_val->media_id ) ||
               ( use_case->media_id == 
               supported_use_case->media_id ) ) &&

               ( ( supported_use_case->vfr_mode == na_val->vfr_mode ) ||
                 ( use_case->vfr_mode == supported_use_case->vfr_mode ) )
           )
           {
              ret_clock_and_bus_preempt_margin_config->vptx_preemption_safety_margin = 
              preemption_margin_level->preemption_safety_margins_values.vptx_preemption_safety_margin;
            
              ret_clock_and_bus_preempt_margin_config->vprx_preemption_safety_margin = 
              preemption_margin_level->preemption_safety_margins_values.vprx_preemption_safety_margin;
              
              ret_clock_and_bus_preempt_margin_config->venc_preemption_safety_margin = 
              preemption_margin_level->preemption_safety_margins_values.venc_preemption_safety_margin;

              ret_clock_and_bus_preempt_margin_config->vdec_preemption_safety_margin = 
              preemption_margin_level->preemption_safety_margins_values.vdec_preemption_safety_margin;

              MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_devcfg_parser: safety margins - vptx: %d, vprx: %d, venc: %d, vdec: %d",
                                                   ret_clock_and_bus_preempt_margin_config->vptx_preemption_safety_margin,
                                                   ret_clock_and_bus_preempt_margin_config->vprx_preemption_safety_margin,
                                                   ret_clock_and_bus_preempt_margin_config->venc_preemption_safety_margin,
                                                   ret_clock_and_bus_preempt_margin_config->vdec_preemption_safety_margin);

              premption_level_use_case_found = TRUE;
              break;
           }
       }
       if( premption_level_use_case_found == TRUE )
       {
         break;
       }
    }
  }
  /* Find the first bus bandwidth in supported use-case table in cvd_devcfg.c that
   * can support the client specified voice use case.
  */
  num_bw_levels = cvd_devcfg_parser_bw_table->num_bw_values;

  for ( bw_level_idx = 0; bw_level_idx < num_bw_levels; ++bw_level_idx )
  {
    bw_level = &cvd_devcfg_parser_bw_table->bw_values[ bw_level_idx ];
    num_supported_use_cases = bw_level->num_supported_use_cases;

    for ( use_case_idx = 0; use_case_idx < num_supported_use_cases; ++use_case_idx )
    {
      supported_use_case = &bw_level->supported_use_cases[ use_case_idx ];

      if ( ( ( supported_use_case->max_num_voice_sessions ==
               na_val->max_num_voice_sessions ) ||
             ( use_case->num_voice_sessions <=
               supported_use_case->max_num_voice_sessions ) ) &&

             ( ( supported_use_case->max_num_nb_streams ==
                 na_val->max_num_nb_streams ) ||
               ( use_case->num_nb_streams <=
                 supported_use_case->max_num_nb_streams ) ) &&

             ( ( supported_use_case->max_num_wb_streams ==
                 na_val->max_num_wb_streams ) ||
               ( use_case->num_wb_streams <=
                 supported_use_case->max_num_wb_streams ) ) &&

             ( ( supported_use_case->max_num_swb_streams ==
                 na_val->max_num_swb_streams ) ||
               ( use_case->num_swb_streams <=
                 supported_use_case->max_num_swb_streams ) ) &&

             ( ( supported_use_case->max_num_fb_plus_streams ==
                 na_val->max_num_fb_plus_streams ) ||
               ( use_case->num_fb_plus_streams <=
                 supported_use_case->max_num_fb_plus_streams ) ) &&

             ( ( supported_use_case->max_num_nb_vocprocs ==
                 na_val->max_num_nb_vocprocs ) ||
               ( use_case->num_nb_vocprocs <=
                 supported_use_case->max_num_nb_vocprocs ) ) &&

             ( ( supported_use_case->max_num_wb_vocprocs ==
                 na_val->max_num_wb_vocprocs ) ||
               ( use_case->num_wb_vocprocs <=
                 supported_use_case->max_num_wb_vocprocs ) ) &&

             ( ( supported_use_case->max_num_swb_vocprocs ==
                 na_val->max_num_swb_vocprocs ) ||
               ( use_case->num_swb_vocprocs <=
                 supported_use_case->max_num_swb_vocprocs ) ) &&

             ( ( supported_use_case->max_num_fb_plus_vocprocs ==
                 na_val->max_num_fb_plus_vocprocs ) ||
               ( use_case->num_fb_plus_vocprocs <=
                 supported_use_case->max_num_fb_plus_vocprocs ) ) &&

             ( ( supported_use_case->tx_topology_id ==
                 na_val->tx_topology_id ) ||
               ( use_case->tx_topology_id ==
                 supported_use_case->tx_topology_id ) ) &&

             ( ( supported_use_case->rx_topology_id ==
                 na_val->rx_topology_id ) ||
               ( use_case->rx_topology_id ==
                 supported_use_case->rx_topology_id ) ) &&

             ( ( supported_use_case->media_id ==
                 na_val->media_id ) ||
               ( use_case->media_id ==
                 supported_use_case->media_id ) ) &&

             ( ( supported_use_case->vfr_mode == na_val->vfr_mode ) ||
               ( use_case->vfr_mode == supported_use_case->vfr_mode ) ) 
            )
      {
        *ret_clock_and_bus_preempt_margin_config->bw_requirement.pBandWidthArray = bw_level->bw_val;

        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): "
               "numOfBw=%d, Bandwidth values: Bytes per sec (%d), Usage percentage (%d)",
               ret_clock_and_bus_preempt_margin_config->bw_requirement.numOfBw,
               (ret_clock_and_bus_preempt_margin_config->bw_requirement.pBandWidthArray->bwValue.busBwValue.bwBytePerSec >> 20 ),
               ret_clock_and_bus_preempt_margin_config->bw_requirement.pBandWidthArray->bwValue.busBwValue.usagePercentage );

        bw_level_use_case_found = TRUE;
        break;
      }
    }

    if( bw_level_use_case_found == TRUE )
    {
      break;
    }
  }

  if( ( clock_level_use_case_found == FALSE ) || ( bw_level_use_case_found == FALSE ) || (premption_level_use_case_found == FALSE))
  {
    if ( clock_level_use_case_found == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): Get clock config failed" );
    }
    if ( bw_level_use_case_found == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): Get bus config failed" );
    }
    if ( premption_level_use_case_found == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config(): Get preemption_safety_margins config failed" );
    }
    return APR_EFAILED;
  }
  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_mmpm_core_info (
  cvd_devcfg_parser_mmpm_core_info_t* ret_mmpm_core_info
)
{
  if ( ret_mmpm_core_info == NULL )
  {
    return APR_EBADPARAM;
  }

  ret_mmpm_core_info->core_id = cvd_devcfg_parser_mmpm_core_info->core_id;
  ret_mmpm_core_info->instance_id = cvd_devcfg_parser_mmpm_core_info->instance_id;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_mmpm_cpp (
  uint32_t* ret_mmpm_cpp
)
{
  if ( ret_mmpm_cpp == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_mmpm_cpp = cvd_devcfg_parser_mmpm_cpp;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_max_q6_core_clock(
  uint32_t* ret_max_q6_core_clock
)
{
  if ( ret_max_q6_core_clock == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_max_q6_core_clock = cvd_devcfg_parser_max_q6_core_clock_mhz;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_min_q6_core_clock(
  uint32_t* ret_min_q6_core_clock
)
{
  if ( ret_min_q6_core_clock == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_min_q6_core_clock = cvd_devcfg_parser_min_q6_core_clock;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_max_bw(
  uint64_t* ret_max_bw
)
{

  if ( ret_max_bw == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_max_bw = cvd_devcfg_parser_max_bw;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_csm_sleep_threshold(
  uint32_t* ret_csm_sleep_threshold
)
{
  if ( ret_csm_sleep_threshold == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_csm_sleep_threshold = cvd_devcfg_parser_csm_sleep_threshold_us;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_csm_safety_margin(
  uint32_t* ret_csm_safety_margin
)
{
  if ( ret_csm_safety_margin == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_csm_safety_margin = cvd_devcfg_parser_csm_safety_margin_us;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_devcfg_parser_get_clk_perf_levels(
  uint32_t* ret_num_clk_perf_levels, uint32_t* ret_clk_perf_levels
)
{
  /* hardcoded for sdm845: to be removed after adding clock levels 
   * extraction thru DAL API */

  *ret_num_clk_perf_levels = cvd_devcfg_parser_num_clk_perf_levels; 

  for(int clk_lvl_idx = 0; clk_lvl_idx < cvd_devcfg_parser_num_clk_perf_levels; clk_lvl_idx++)
  {
    ret_clk_perf_levels[ clk_lvl_idx ] = cvd_devcfg_parser_clk_perf_levels_khz[ clk_lvl_idx ];
  }

  return APR_EOK;
}


#endif /* !WINSIM */

