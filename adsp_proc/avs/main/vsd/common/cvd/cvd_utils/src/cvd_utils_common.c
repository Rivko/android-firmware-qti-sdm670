/*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_utils_common.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"
#include "mmstd.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "vss_common_public_if.h"
#include "cvd_mem_mapper_i.h"
#include "vss_private_if.h"
#include "cvd_cal_private_i.h"
#include "cvd_vfr_api.h"
#include "mvm_api_i.h"

/*****************************************************************************
 * COMMON ROUTINES                                                             *
 ****************************************************************************/

APR_INTERNAL bool_t is_cvd_utils_obj_freed( void )
{
  uint32_t obj_used_count = 0;
  bool_t result = TRUE;
  
  ( void )cvd_mem_mapper_obj_stat( &obj_used_count );
  if( obj_used_count > 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "is_cvd_utils_obj_freed(): Objects "
                                            "active in cvd mem mapper:(%d)", obj_used_count );
    result = FALSE;
  }
  
  ( void )cvd_cal_obj_stat( &obj_used_count );
  if( obj_used_count > 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "is_cvd_utils_obj_freed(): Objects "
                                            "active in cvd cal:(%d)", obj_used_count );
    result = FALSE;
  }

#if 0
  ( void )cvd_vfr_call( CVD_VFR_CMDID_OBJ_STAT, &obj_used_count, sizeof( obj_used_count) );
  if( obj_used_count > 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "is_cvd_utils_obj_freed(): Objects "
                                            "active in cvd vfr:(%d)", obj_used_count );
    result = FALSE;
  }
#endif

  ( void )mvm_call( MVM_CMDID_OBJ_STAT, &obj_used_count, sizeof( obj_used_count) );
  if( obj_used_count > 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "is_cvd_utils_obj_freed(): Objects "
                                            "active in mvm:(%d)", obj_used_count );
    result = FALSE;
  }
  
  return result;
}
