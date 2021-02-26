/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/inc/bgt_handler.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains Background Task Handler header. This file should NOT be imported by
   CAPI V2 Modules (they should use bgt_handler_module.h instead). This file is to be used
   by the bgt_handler.cpp implementation file as well as any service layer framework code.
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/
#ifndef __BGT_HANDLER_H__
#define __BGT_HANDLER_H__

/*==========================================================================
  Include files
  ========================================================================== */
#include "Elite.h"
#include "Elite_fwk_extns_background_processing.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Main structure of the BGT */
struct module_bgt_info_t
{
   /* Foreground thread data pointer, this is used only for init call and should NOT be used
    * thereafter, in any of the process, deinit and destroy calls. */
   capi_v2_fgt_data_t        *fgt_data_ptr;

   /* Service cmd Q pointer that will handle the thread destroy cmd*/
   qurt_elite_queue_t        *svc_cmd_q_ptr;
   /* Service destroy opcode to destroy BGT thread */
   uint32_t                  svc_bgt_destroy_opcode;

   /* The service handle that contains the Qs */
   elite_svc_handle_t        bgt_svc_handle;
   /* Channels used by bgt handler */
   qurt_elite_channel_t      channel;
   /* Signal used by bgt handler */
   qurt_elite_signal_t       signal;
   /* ThreadID of each bgt handler thread */
   qurt_elite_thread_t       threadID;
   /* To track the BGT module data memory*/
   capi_v2_bgt_data_t        *bgt_data_ptr;
   /* The response Q for BGT */
   qurt_elite_queue_t        *resp_q_ptr;
   /* The Buffer Q for BGT used for communication b/w BGT and FGT
    * FGT takes buffer from this Q and writes its processed data onto this
    * and pushes it onto the data Q
    */
   qurt_elite_queue_t        *buf_q_ptr;
   /* Number of buffers created in the Buf Q */
   uint32_t                  num_buffs_created;
   /* Module specific info */
   capi_v2_bgt_module_info_t bgt_module_info;

   /* If the module also uses feedback path, then we need to destroy in BGT. */
   uint32_t                  src_id;
   uint32_t                  dest_id;
   uint32_t                  feedback_handle;
};


#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __BGT_HANDLER_H__
