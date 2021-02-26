/*========================================================================
  This file contains AFE Error handler related API's

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: 
 ====================================================================== */
#ifndef _AFE_ERR_HDLR_H_
#define _AFE_ERR_HDLR_H_

/*==========================================================================
  Include Files
  ========================================================================== */

#include "qurt_elite.h"
#include "AFEInternal.h"
#include "AFEErrorHandler_debug.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

/**<afe error handler driver handle.*/
typedef void* afe_err_handler_drv_handle_t;


/*==========================================================================
  Function declarations
  ========================================================================== */

/**
 * AFE error handler
 *
 * @param[in] port_id, afe port id
 * @returns ADSP_EOK, error code
 */
ADSPResult afe_err_handler(uint32_t port_id);

/**
 * This function de-registers afe error handler
 *
 * @param[in] err_handler_h, pointer to error handler handle
 * @return ADSP_EOK, error code
 */
ADSPResult afe_err_handler_driver_deregister(afe_err_handler_drv_handle_t *err_handler);

/**
 * This function registers afe port to have afe error handler
 *
 * @param[in] err_handler_h, pointer to error handler handle
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_handler_driver_register(afe_err_handler_drv_handle_t *err_handler_handle);

/**
 * AFE service thread calls (through cmdQ) this function
 *  to invoke device error recovery
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg from port cmdQ
 * @return the ADSP_EOK, error code
*/
ADSPResult afe_err_handler_dev_recovery_msg(afe_dev_port_t *pDevPort, elite_msg_any_t *pMsg);



/**
 * Performs afe error handler driver initialization.
 * 			Initializes the content of global signal miss
 * 			information pool to zero.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_init();

/**
 * Performs afe error handler driver de-initialization.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_deinit();

#endif /* _AFE_ERR_HDLR_H_ */
