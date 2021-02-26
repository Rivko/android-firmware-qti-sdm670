#ifndef __CVD_CAL_PROTECTED_I_H__
#define __CVD_CAL_PROTECTED_I_H__

/*
   Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/cvd_cal_protected_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_CAL_PARAM_MINOR_VERSION_0 ( 0 )

#define CVD_CAL_PARAM_MINOR_VERSION_1 ( 1 )

typedef struct cvd_cal_param_t cvd_cal_param_t;

#include "apr_pack_begin.h"

struct cvd_cal_param_t
{
  uint32_t minor_version; 
/**
  * supported versions:
  *   CVD_CAL_PARAM_MINOR_VERSION_0
*/
  //payload starts here
  uint32_t module_id;
  uint32_t param_id;
  uint32_t param_data_size;
  void* param_data;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_param_v1_t cvd_cal_param_v1_t;

#include "apr_pack_begin.h"

struct cvd_cal_param_v1_t
{
  uint32_t minor_version; 
/**
  * supported versions: 
  *   CVD_CAL_PARAM_MINOR_VERSION_1
*/
  //payload starts here
  uint32_t module_id;
  uint16_t instance_id;
  uint16_t reserved;
  uint32_t param_id;
  uint32_t param_data_size;
  void* param_data;
}
#include "apr_pack_end.h"
;

typedef struct cvd_rx_gain_cal_param_t cvd_rx_gain_cal_param_t;

#include "apr_pack_begin.h"

struct cvd_rx_gain_cal_param_t
{
    uint32_t minor_version;
    /**supported versions:CVD_CAL_PARAM_MINOR_VERSION_0*/
    //payload starts here
    uint32_t module_id;
    uint32_t param_id;
    uint32_t param_data_size;
    uint32_t param_data;
}
#include "apr_pack_end.h"
;

typedef struct cvd_rx_gain_cal_param_v1_t cvd_rx_gain_cal_param_v1_t;

#include "apr_pack_begin.h"

struct cvd_rx_gain_cal_param_v1_t
{
    uint32_t minor_version;
    /**supported versions:CVD_CAL_PARAM_MINOR_VERSION_1*/
    //payload starts here
    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved;
    uint32_t param_id;
    uint32_t param_data_size;
    uint32_t param_data;    
}
#include "apr_pack_end.h"
;


/**
  Calibration query callback function prototype.

  @param[in] cal_param: A calibration parameter to apply.
  @param[in] client_data: Pointer to a token/data supplied by the client 
                      through MVM_CMDID_CAL_QUERY .

*/
  typedef void ( *cvd_cal_query_cb_fn_t ) ( void* cal_params,
                                            void* client_data );

#endif /* __CVD_CAL_PROTECTED_I_H__ */
