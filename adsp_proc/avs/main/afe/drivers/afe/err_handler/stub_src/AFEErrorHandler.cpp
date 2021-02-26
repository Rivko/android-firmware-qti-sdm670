/*========================================================================
   This file contains AFE Error handler stub implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: 
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "qurt_elite.h"
#include "AFEErrorHandler.h"

/** Global pointer to access signal miss info pool */
afe_signal_miss_info_t * signal_miss_pool_global_ptr;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
 
ADSPResult afe_err_handler(uint32_t port_id)
{
   return ADSP_EOK;
}

ADSPResult afe_err_handler_driver_deregister(afe_err_handler_drv_handle_t *err_handler_h)
{
  if(NULL != err_handler_h)
  {
    *err_handler_h = NULL;
  }

  return ADSP_EOK;
}

ADSPResult afe_err_handler_driver_register(afe_err_handler_drv_handle_t *err_handler_h)
{
  if(NULL != err_handler_h)
  {
    *err_handler_h = NULL;
  }

  return ADSP_EOK;
}

ADSPResult afe_err_handler_dev_recovery_msg(afe_dev_port_t *pDevPort, elite_msg_any_t *pMsg)
{
   return ADSP_EOK;
}


ADSPResult afe_err_handler_init()
{
  /*< Initializes global signal miss pool global pointer.*/
  signal_miss_pool_global_ptr = NULL;

  return ADSP_EUNSUPPORTED;
}

ADSPResult afe_err_handler_deinit()
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_err_handler_set_dev_recovery_cfg(int8_t *param_buffer_ptr, uint16_t  param_size)
{
  return ADSP_EOK;
}

ADSPResult afe_err_handler_set_port_signal_miss_cfg(afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, uint32_t param_id)
{
  return ADSP_EOK;
}

ADSPResult afe_err_handler_free_port_signal_miss_cfg(afe_dev_port_t * pDevPort)
{
  return ADSP_EOK;
}

ADSPResult afe_err_handler_force_signal_miss(afe_dev_port_t *pDevPort)
{
  return ADSP_EOK;
}
