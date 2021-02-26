#ifndef __VRM_CONFIG_H__
#define __VRM_CONFIG_H__

/*! \file vrm_config.h
*
*  \brief This header file contains internal VRM config related definitions.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/vrm_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_inc.h"
#include "vrm_hwio.h"
#include "vrm_config_target.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_MAX_NUM_CHILDREN         16
#define VRM_MAX_NUM_PARENTS          5
#define VRM_MAX_SETTLING_TIME_USEC   3400

//num of sequencers
#define VRM_MAX_NUM_SEQCERS   HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_MAXr+1
//max seq id
#define VRM_INVALID_SEQ_ID    HWIO_RPMH_VRM_SEQ_START_ADDRr_MAXr+1

typedef struct
{
   cmd_db_slv_id_type vrm_slave_id;
   uint32  min_settings_drv;
   uint32* seq_id_arr;
   uint32  soc_pbs_sleep_arg;
   uint32  soc_pbs_wake_arg;
}vrm_config_data;

//******************************************************************************
// API Functions
//******************************************************************************

vrm_config_data* vrm_get_config_data(void);

#endif
