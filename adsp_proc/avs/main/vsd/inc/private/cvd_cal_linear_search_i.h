#ifndef __CVD_CAL_LINEAR_SEARCH_I_H__
#define __CVD_CAL_LINEAR_SEARCH_I_H__

/*
   Copyright (C) 2012 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_cal_linear_search_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/


/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

APR_INTERNAL int32_t cvd_cal_linear_search_query (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
);

APR_INTERNAL int32_t cvd_cal_linear_search_query_v2 (
  cvd_cal_table_object_t* table_object,
  cvd_cal_key_t* key,
  uint8_t* matched_entries_buffer,
  uint32_t matched_entries_buffer_size,
  uint32_t* ret_num_entries
);

#endif /* __CVD_CAL_LINEAR_SEARCH_I_H__ */
