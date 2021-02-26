/*==============================================================================
  FILE:         pdc_seq.c
  
  OVERVIEW:     This file implements some of the APIs defined in PDC Sequencer public
                header file.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/seq/common/pdc_seq.c#2 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"
#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "CoreVerify.h"

/*==============================================================================
                                LOCAL FUNCTIONS
 =============================================================================*/
/**
 * pdc_seq_init
 *
 * @brief Performs PDC sequencer instance specific initialization.
 *
 * @note This function must be called on a handle/instance before calling
 *       any other APIs that take handle as input.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
static pdc_seq_result_t pdc_seq_init(void)
{
  pdc_seq_result_t result;

  result = pdc_seq_handle_internal_cfg();

  if (result != PDC_SEQ_SUCCESS)
  {
    return result;
  }
 
  result = pdc_seq_copy_cmd_seq(); 

  return result;
}

/*==============================================================================
                               GLOBAL FUNCTIONS
 =============================================================================*/
/*
 * pdc_set_wakeup_time
 */
pdc_seq_result_t pdc_seq_set_wakeup_time(uint64 wakeup)
{
  /* Preferred method is through RSC but PDC may not always have RSC e.g
   * for debug subsystem */
  if(g_pdcSeq->rsc_addr != NULL)
  {
    pdc_seq_hal_cfg_wakeup_time(g_pdcSeq->rsc_addr, wakeup);
  }
  else
  {
    pdc_seq_hal_cfg_wakeup_time_ex(g_pdcSeq->addr, wakeup);
  }

  return PDC_SEQ_SUCCESS;
}

/*
 * pdc_seq_set_lpm
 */
pdc_seq_result_t pdc_seq_set_lpm(pdc_low_power_mode_type modeId)
{
  int                     modeIndex;
  pdc_seq_hal_bsp_mode_t *mode;

  modeIndex = pdc_seq_mode_index(modeId);
  if(modeIndex == PDC_SEQ_UNSUPPORTED_MODE)
  {
    return PDC_SEQ_UNSUPPORTED_MODE;
  }

  mode = &g_pdcSeq->modes[modeIndex];

  /* Preferred method is through RSC but PDC may not always have RSC e.g
   * for debug subsystem */
  if (g_pdcSeq->rsc_addr != NULL)
  {
    pdc_seq_hal_cfg_start_addr(g_pdcSeq->rsc_addr, mode->start_addr);
  }
  else
  {
    pdc_seq_hal_cfg_start_addr_ex(g_pdcSeq->addr, mode->start_addr);
  }

  return PDC_SEQ_SUCCESS;
}

/**
 * pdc_seq_sys_init
 *
 * @brief This function initializes common PDC sequencer driver as well
 *        as all the PDC instances supported on a given subsystem.
 */
void pdc_seq_sys_init(void)
{
  /* Target Initializations */
  pdc_seq_target_init();

  /* Sequence init based on target */
  CORE_VERIFY(PDC_SEQ_SUCCESS == pdc_seq_init());

  /* Sequencer enable */
  pdc_seq_hal_enable(g_pdcSeq->addr, 1);

  return;
}

