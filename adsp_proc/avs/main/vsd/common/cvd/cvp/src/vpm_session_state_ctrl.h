/**========================================================================
 @file vpm_session_state_ctrl.h
 
 @brief This header file contains function declarations for vocproc session
 object state control utilities.
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_state_ctrl.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SESSION_STATE_CTRL_H_
#define _VPM_SESSION_STATE_CTRL_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */

ADSPResult vpm_state_control(vpm_session_object_t *session_obj);

#endif /* #ifndef _VPM_SESSION_STATE_CTRL_H_ */

