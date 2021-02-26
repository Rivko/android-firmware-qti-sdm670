#pragma once
/*============================================================================
  @file sns_sdc.h

  @brief utility for sdc init and image loading

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/**
*****************************************************************************************
                        Public Function Declarations
*****************************************************************************************
*/
/**
 * API to convert SDC address to Virtual address on SLPI
 */
uint32_t (*sns_sdc_get_vaddr)( uint32_t addr );

/**
 * Control the SDC Bus HCLK
 * */
void (*sns_sdc_tcm_clk_config)( bool awake );

/**
 * Load sdc code and data binaries
 */
sns_rc sns_load_sdc_bins( void );

