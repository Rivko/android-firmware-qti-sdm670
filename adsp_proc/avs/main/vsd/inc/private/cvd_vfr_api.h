#ifndef __CVD_VFR_API_H__
#define __CVD_VFR_API_H__

/*
   Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_vfr_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "drv_api.h"

APR_EXTERNAL uint32_t cvd_vfr_subscribe_to_vtm( uint32_t vfr_cycle_duration_us );

APR_EXTERNAL uint32_t cvd_vfr_unsubscribe_to_vtm( void );

APR_EXTERNAL uint32_t cvd_vfr_get_voice_frame_reference( 
  uint64_t *vfrTimestamp
);

APR_EXTERNAL int32_t cvd_vfr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /* __CVD_VFR_API_H__ */

