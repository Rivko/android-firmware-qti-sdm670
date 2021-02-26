/**========================================================================
 @file vpm_session_cal_utils.h
 
 @brief This header file contains function declarations for vocproc session
 object calibration utilities.
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_cal_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SESSION_CAL_UTILS_H_
#define _VPM_SESSION_CAL_UTILS_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */


ADSPResult vpm_find_vol_table_format (vpm_session_object_t* session_obj,
                                      bool_t* is_v1_format);

ADSPResult vpm_calibrate_volume(vpm_session_object_t *session_obj,
                                uint32_t vol_ramp_duration);

ADSPResult vpm_calibrate_common(vpm_session_object_t* session_obj);

ADSPResult vpm_calibrate_dynamic (vpm_session_object_t *session_obj,
                                  uint32_t vol_ramp_duration,
                                  vpm_dynamic_cal_usecase_enum_t usecase);

ADSPResult vpm_calibrate_static(vpm_session_object_t *session_obj);

ADSPResult vpm_helper_search_hdvoice_config(vpm_session_object_t *session_obj,
                                            vss_ihdvoice_cmd_get_config_t *search_values,
                                            vss_ihdvoice_rsp_get_config_t *ret_data_entry);


ADSPResult vpm_action_set_soundfocus_sectors(vpm_session_object_t *session_obj);


ADSPResult vpm_action_set_ui_properties(vpm_session_object_t *session_obj);

ADSPResult vpm_validate_calib_payload(uint32_t mem_handle,
                                      uint32_t mem_size,
                                      uint64_t mem_address);

#endif /* #ifndef _VPM_SESSION_CAL_UTILS_H_ */


