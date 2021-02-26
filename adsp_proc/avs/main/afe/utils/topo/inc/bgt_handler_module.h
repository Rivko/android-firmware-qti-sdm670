/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/inc/bgt_handler_module.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains Background Task Handler implementation to be used by CAPI V2 modules.
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/
#ifndef __BGT_HANDLER_MODULE_H__
#define __BGT_HANDLER_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*==========================================================================
  Include files
  ========================================================================== */

#include "qurt_elite_types.h"
#include "Elite_fwk_extns_background_processing.h"

/*==========================================================================
  Function Declarations
========================================================================== */
/**
 * This function creates the buffers and pushes them into the buffer Q
 */
ADSPResult bgt_create_buffers(module_bgt_info_t *bgt_info_ptr, uint32_t num_buffers, uint32_t buf_size);

/**
 * This function destroys the buffers, should be called during module BGT de-init
 */
void bgt_destroy_buffers(module_bgt_info_t *bgt_info_ptr);

/**
 * This function pops a buffer from the buffer Q
 */
ADSPResult bgt_pop_buf_from_buf_q(module_bgt_info_t *bgt_info_ptr, void **data_ptr_ptr);

/**
 * This function pops a buffer from the data Q
 */
ADSPResult bgt_pop_buf_from_data_q(module_bgt_info_t *bgt_info_ptr, void **data_ptr_ptr);

/**
 * This function sends the buffer to the data Q
 */
ADSPResult bgt_send_buf_to_data_q(module_bgt_info_t *bgt_info_ptr, void *data_ptr);

/**
 * This function returns the buffer to the buf Q
 */
ADSPResult bgt_return_buf_to_buf_q(module_bgt_info_t *bgt_info_ptr, void *data_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __BGT_HANDLER_MODULE_H__
