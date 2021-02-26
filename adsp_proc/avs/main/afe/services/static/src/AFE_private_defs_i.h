/*========================================================================
  This file contains private API definitions.

  Copyright (c) 2013-2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*
 * AFE_private_defs_i.h
 *
 *  Created on: Aug 11, 2013
 *      Author: rajkrish
 */

#ifndef AFE_PRIVATE_DEFS_I_H_
#define AFE_PRIVATE_DEFS_I_H_

#define AFE_PORT_GET_DRIFT_PARAM_AND_VERIFY              0xFFFF0000

#define AFE_MODULE_ID_DRIFT                              0xFFFF0001

#define AFE_PARAM_ID_AVT_DRFT                            0xFFFF0002

#define AFE_MODULE_DEV_CONFIG                            0xFFFF0004
/**< (Service specific) Module ID used for AFE service configuration
 * For example, In order to configure device recovery params
 * (which is service specific), uses this module ID.*/

#define AFE_PARAM_ID_DEVICE_RECOVERY_CONFIG              0xFFFF0005
/**< (Service specific) Param ID used to configure device recovery
 *  parameters max_num_allowed_recoveries and window_interval.*/

typedef struct afe_avt_drift_param_t  afe_avt_drift_param_t;

struct afe_avt_drift_param_t
{
   uint32_t frame_counter;

   int32_t drift_value;
};

#define AFE_PARAM_ID_VFR_DRFT                            0xFFFF0003

typedef struct afe_vfr_drift_param_t  afe_vfr_drift_param_t;

struct afe_vfr_drift_param_t
{
   uint32_t frame_counter[NUM_MAX_VFR_SRC];

   uint32_t drift_value[NUM_MAX_VFR_SRC];
};

#endif /* AFE_PRIVATE_DEFS_I_H_ */
