#ifndef __CVD_DEVCFG_PARSER_I_H__
#define __CVD_DEVCFG_PARSER_I_H__

/*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_devcfg_parser_i.h#2 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vss_public_if.h"

#ifndef WINSIM
  #include "mmpm.h"
#endif /* !WINSIM */

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef struct cvd_devcfg_parser_voice_use_case_t cvd_devcfg_parser_voice_use_case_t;

struct cvd_devcfg_parser_voice_use_case_t
{
  uint16_t num_voice_sessions;
    /**< Number of active voice sessions in the system. */
  uint16_t num_nb_streams;
    /**< Number of narrow-band (8 KHz) streams in the system. */
  uint16_t num_wb_streams;
    /**< Number of wide-band (16 KHz) streams in the system. */
  uint16_t num_swb_streams;
    /**< Number of super wide-band (32 KHz) streams in the system. */
  uint16_t num_fb_plus_streams;
    /**< Number of full-band (48 KHz) or higher streams in the system. */
  uint16_t num_nb_vocprocs;
    /**< Number of narrow-band (8 KHz) vocprocs in the system. */
  uint16_t num_wb_vocprocs;
    /**< Number of wide-band (16 Khz) vocprocs in the system. */
  uint16_t num_swb_vocprocs;
    /**< Number of super wide-band (32 KHz) vocprocs in the system. */
  uint16_t num_fb_plus_vocprocs;
    /**< Number of full-band (48 Khz) or higher vocprocs in 
         the system. */
  uint32_t tx_topology_id;
    /**< Tx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t rx_topology_id;
    /**< Rx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t media_id;
    /**< Media ID. Applicable only if there is a single active  
         voice session which has a single vocproc and/or stream attached to it. 
         It is ignored otherwise. */
  uint16_t vfr_mode;
    /**< The VFR mode. Applicable only if there is a single active voice 
         session in the system, which has a single vocproc and/or stream 
         attached to it. It is ignored otherwise.
         The supported values:\n
         #VSS_ICOMMON_VFR_MODE_SOFT \n
         #VSS_ICOMMON_VFR_MODE_HARD */
};

#ifndef WINSIM

typedef struct cvd_devcfg_parser_mmpm_core_info_t cvd_devcfg_parser_mmpm_core_info_t;

struct cvd_devcfg_parser_mmpm_core_info_t
{
  MmpmCoreIdType core_id;
    /**< Indicate which H/W core to use. */
  MmpmCoreInstanceIdType instance_id;
    /**< Indicate which instance of the same H/W core. */
};

typedef struct cvd_devcfg_parser_target_specific_config_t cvd_devcfg_parser_target_specific_config_t;

struct cvd_devcfg_parser_target_specific_config_t
{
  uint32_t core_floor_clock_hz;
    /**< Core VDSP Floor clock in Hz. */
  uint32_t cycles_per_thousand_instr_packets;
    /**< Number of clock cycles required to execute 1000 instruction packets. */
  MmpmGenBwReqType bw_requirement;
    /**< Required bandwidth. */
  uint32_t sleep_latency_us;
    /**< Required sleep latency in microseconds. */
  uint32_t vptx_preemption_safety_margin;
    /**< vptx preemption margin value in microseconds. */
  uint32_t vprx_preemption_safety_margin;
    /**< vprx preemption margin value in microseconds. */
  uint32_t venc_preemption_safety_margin;
    /**< venc preemption margin value in microseconds. */
  uint32_t vdec_preemption_safety_margin; 
    /**< vdec preemption margin value in microseconds. */
};

#endif /* !WINSIM */

/****************************************************************************
 * PROTOTYPES                                                               *
 ****************************************************************************/

/** 
  Initialize the CVD devcfg parser. Must be called before any other CVD devcfg
  APIs can be called, and must be called only once.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_init ( void );

/** 
  Deinitialize the CVD devcfg parser.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_deinit ( void );

#ifndef WINSIM

/** 
  Retrieve the clock and bus configurations for the client specified voice use
  case. Based on the returned configurations, the client can then request 
  resources from MMPM.

  @param[in] use_case                  Client specified voice use case.
  @param[out] ret_clock_and_bus_config Returns the clock and bus configurations.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config ( 
  cvd_devcfg_parser_voice_use_case_t* use_case,
  cvd_devcfg_parser_target_specific_config_t* ret_clock_and_bus_preempt_margin_config
);

/** 
  Retrieve the MMPM HW core and core instance information to be used for MMPM 
  registration.

  @param[out] ret_mmpm_core_info Returns the HW core and core instance information.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_mmpm_core_info ( 
  cvd_devcfg_parser_mmpm_core_info_t* ret_mmpm_core_info
);

/** 
  Retrieve the MMPM cycles per instruction packet information to
  be used for MMPM registration. 

  @param[out] ret_mmpm_cpp Returns the MMPM cycles per 
              instruction packet information.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_mmpm_cpp (
  uint32_t* ret_mmpm_cpp
);

/** 
  Retrieve the core Q6 clock information to
  be used for MMPM registration. 

  @param[out] ret_max_q6_core_clock Returns the max Q6 core  
              clock in MHz.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_max_q6_core_clock(
  uint32_t* ret_max_q6_core_clock
);

/** 
  Retrieve the lowest core Q6 clock information from cvd_devcfg clcok table. 

  @param[out] ret_min_q6_core_clock returns the minimal/SVS Q6 core  
              clock in MHz

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_min_q6_core_clock(
 uint32_t* ret_min_q6_core_clock
);

/** 
  Retrieve the Max Bandwidth required for voice to be used for
  MMPM registration. 

  @param[out] Returns the max bus clock required for voice.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_max_bw(
  uint64_t* ret_max_bw
);

/** 
  Retrieve the sleep threshold for CSM to be used for dynamic 
  voting. 

  @param[out] Returns the sleep threshold for CSM.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_csm_sleep_threshold(
  uint32_t* ret_csm_sleep_threshold
);

/** 
  Retrieve the safety margin for CSM to be used for dynamic 
  voting. 

  @param[out] Returns the safety margin for CSM.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_csm_safety_margin(
  uint32_t* ret_csm_safety_margin
);

/** 
  Retrieve the clock levels for CCM to be used for clock 
  voting. 

  @param[out] Returns the number of clock levels.
  @param[out] Returns the clock levels.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_clk_perf_levels(
  uint32_t* ret_num_clk_perf_levels, uint32_t* ret_clk_perf_levels
);

#endif /* !WINSIM */

#endif /* __CVD_DEVCFG_PARSER_I_H__ */

