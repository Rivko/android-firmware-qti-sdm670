/**========================================================================
 @file vpm_svc_vp3_utils.h
 
 @brief This header file contains function declarations for VP3 configuration
 utilites
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_vp3_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SVC_VP3_UTILS_H_
#define _VPM_SVC_VP3_UTILS_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */


ADSPResult vpm_svc_vp3_init(vpm_vp3_info_t  *p_vp3_info);

ADSPResult vpm_svc_vp3_deinit(vpm_vp3_info_t  *p_vp3_info); 

ADSPResult vpm_update_global_vp3_db(vpm_session_object_t *session_obj);

ADSPResult vpm_action_set_vp3(vpm_session_object_t *session_obj);

ADSPResult vpm_action_get_vp3(vpm_session_object_t *session_obj);

ADSPResult vpm_vp3_get_data_cmd_ctrl(vpm_session_object_t *session_obj);

ADSPResult vpm_vp3_set_data_cmd_ctrl(vpm_session_object_t *session_obj);

ADSPResult vpm_vp3_get_size_cmd_ctrl(vpm_session_object_t *session_obj);

#endif /* #ifndef _VPM_SVC_VP3_UTILS_H_ */

