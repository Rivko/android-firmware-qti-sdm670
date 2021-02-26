/*========================================================================
  This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2015 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/inc/afe_lpass_core_clk.h#1 $
 ====================================================================== */

#ifndef _AFE_LPASS_CORE_CLK_H_
#define _AFE_LPASS_CORE_CLK_H_

#include "Elite.h"

/*==========================================================================
  Include Files
========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*
 * @brief Init function for lpass core clock
 *@param[in] hw_version hardware version of LPASS
 *
 *@return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_lpass_core_clk_init(uint32_t hw_version);


/*
 * DeInit function for lpass core clock
 
 *@return ADSP_EOK on success, an error code on error

*/
ADSPResult afe_lpass_core_clk_deinit(void);

/*
 * @brief Enables/Disables lpass core clock
 
 * @param[in] cfg_ptr   pointer to the configuration structure
 * @param[in] payload_size   size of the configuration payload
 * 
 * @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_lpass_core_clk_set(int8_t *cfg_ptr, uint32_t payload_size);

/* This function registers with MMPM 
 * Added this function separately in order to compile this code on
 * Bear Targets. On bear targets like 8909 this MMPM core id not supported. 
 *  @result
 *  mmpm client id for success , 0 for failure
 * */ 
         
uint32_t afe_mmpm_register_lpass_core();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif

