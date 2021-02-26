#ifndef __CVD_CAL_COMMON_I_H__
#define __CVD_CAL_COMMON_I_H__

/*
   Copyright (c) 2012-2013, 2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_cal_common_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "apr_memmgr.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_CAL_QUERY_MATCH ( 0 )
#define CVD_CAL_QUERY_NOT_MATCH ( 1 )

/****************************************************************************
 * COMMON UTILITIES                                                         *
 ****************************************************************************/

/* Get width in bytes of a data type. */
APR_INTERNAL uint32_t cvd_cal_get_type_width (
  uint32_t data_type
);

/* Get the size/data pair information from an entry in table,
   return the total size of this entry. */
APR_INTERNAL uint32_t cvd_cal_get_entry_info (
  cvd_cal_table_handle_t* table_handle,
  uint8_t* cur_ptr,
  cvd_cal_entry_t* entry_item
);

/* Get the meta size (size excluding the actual data and data size)
   of a cal entry in table. */
APR_INTERNAL int32_t cvd_cal_get_entry_meta_size ( 
  cvd_cal_column_descriptor_t* column_descriptor,
  uint32_t num_columns,
  uint32_t* ret_size 
);

/* Get number of cal entries in from a cal table. */
APR_INTERNAL int32_t cvd_cal_get_num_cal_entries (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t* ret_num_cal_entries
);

/* Form a new key by re-ordering a query key. 
   Columns in new key has the same order as in calibration table. 
   N/A values are filled into new key if the original query 
   key does not have all the columns present. */
APR_INTERNAL int32_t cvd_cal_reorder_key (
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_key_t* orig_key,
  cvd_cal_key_t* ret_key,
  uint8_t* column_buf
 );


/* Validates that the indexing mem map handle is valid, that the size
   lies within the mapped region, and that pointers are aligned to 32 byte boundaries. */
APR_INTERNAL int32_t cvd_cal_validate_indexing_mem (
  cvd_cal_table_descriptor_t* table_descriptor, 
  void* indexing_ptr, 
  uint32_t indexing_size
);

/* Validates that the current mem map handle is valid, that the data size
   lies within the mapped region, and that pointers are aligned to 4 byte boundaries. */
APR_INTERNAL int32_t cvd_cal_validate_entry (
  cvd_cal_table_descriptor_t* table_descriptor, 
  void* entry_ptr, 
  uint32_t data_size
);

/* Validates that the current mem map handle is valid and that the data size
   lies within the mapped region */
APR_INTERNAL int32_t cvd_cal_validate_table (
  cvd_cal_table_descriptor_t* table_descriptor
);

/* Gets the minor version of a given cal table. */
APR_INTERNAL int32_t cvd_cal_get_minor_version (
  uint32_t table_handle,
  uint32_t* minor_version
);

#endif /* __CVD_CAL_COMMON_I_H__ */
