/**========================================================================
 @file vpm_response_handler.h
 
 @brief This header file contains function declarations for response handlers
 for static service and vocproc session object response queues.
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_response_handler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_RESPONSE_HANDLER_H_
#define _VPM_RESPONSE_HANDLER_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */

ADSPResult vpm_vp3_get_size_cmd_ctrl_rsp_fn(void *contex_ptr);

ADSPResult vpm_vp3_get_data_cmd_rsp_fn(void *context_ptr);

ADSPResult vpm_get_vp3_param_transition_rsp_fn(void *context_ptr);

ADSPResult vpm_get_kpps_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_delay_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_param_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_ui_prop_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_ui_prop_v2_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_param_v3_cmd_rsp_handler(void *context_ptr);

ADSPResult vpm_get_soundfocus_sectors_cmd_rsp_fn(void *context_ptr);

ADSPResult vpm_sourcetrack_get_activity_cmd_rsp_fn(void *context_ptr);

ADSPResult vpm_get_param_cmd_rsp_fwd(void *context_ptr);

ADSPResult vpm_get_param_v3_cmd_rsp_fwd(void *context_ptr);

ADSPResult vpm_set_vp3_param_transition_rsp_fn(void *context_ptr);

#endif /* #ifndef _VPM_RESPONSE_HANDLER_H_ */

