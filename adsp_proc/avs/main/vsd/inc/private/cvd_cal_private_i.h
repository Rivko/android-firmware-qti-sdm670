#ifndef __CVD_CAL_PRIVATE_I_H__
#define __CVD_CAL_PRIVATE_I_H__

/*
   Copyright (c) 2012-2014, 2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_cal_private_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"

#include "cvd_cal_protected_i.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Max number of colunms is 5. */
#define CVD_CAL_KEY_COLUMN_SIZE_IN_BYTES ( 80 )

#define CVD_CAL_GET_REQUIRED_MATCHING_ENTRIES_BYTE_SIZE( num_columns ) \
  ( ( 1 << num_columns ) * 4 )

#define CVD_CAL_INVALID_HANDLE_V APR_NULL_V

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef union cvd_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
cvd_cal_na_value_t;

typedef struct cvd_cal_column_descriptor_t cvd_cal_column_descriptor_t;

#include "apr_pack_begin.h"

struct cvd_cal_column_descriptor_t
{
  uint32_t id;
  uint32_t type;
  cvd_cal_na_value_t na_value;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_table_descriptor_t cvd_cal_table_descriptor_t;

#include "apr_pack_begin.h"

struct cvd_cal_table_descriptor_t
{
  void* start_ptr;
  uint32_t size;
  uint32_t data_mem_handle;
  uint32_t indexing_mem_handle;
  uint32_t num_columns;
  cvd_cal_column_descriptor_t* columns;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_column_t cvd_cal_column_t;

#include "apr_pack_begin.h"

struct cvd_cal_column_t
{
  uint32_t id;
  uint32_t value; /* Currently supported max column width is 32 bits. */
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_table_handle_t cvd_cal_table_handle_t;

#include "apr_pack_begin.h"

struct cvd_cal_table_handle_t
{
  cvd_cal_table_descriptor_t table_descriptor;

  uint8_t reorder_key_buf[ CVD_CAL_KEY_COLUMN_SIZE_IN_BYTES ];
  void* aux_info;

  uint32_t meta_size;
  bool_t is_parsed;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_key_t cvd_cal_key_t;

#include "apr_pack_begin.h"

struct cvd_cal_key_t
{
  uint32_t num_columns;
  cvd_cal_column_t* columns;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_entry_t cvd_cal_entry_t;

#include "apr_pack_begin.h"

struct cvd_cal_entry_t
{
  void* start_ptr;
  uint32_t size;
}
#include "apr_pack_end.h"
;

/* Following data types are to be used with compressed calibration table format.
*/

typedef enum cvd_cal_object_type_enum_t
{
  CVD_CAL_OBJECT_TYPE_ENUM_UNINITIALIZED,
  CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
  CVD_CAL_OBJECT_TYPE_ENUM_QUERY,
  CVD_CAL_OBJECT_TYPE_ENUM_INVALID
}
  cvd_cal_object_type_enum_t;

typedef struct cvd_cal_object_header_t
{
  uint32_t handle;
    /**< The handle to the associated apr_objmgr_object_t instance. */
  cvd_cal_object_type_enum_t type;
    /**<
     * The object type defines the actual derived object.
     *
     * A free object entry is set
     * to CVD_CAL_OBJECT_TYPE_ENUM_UNINITIALIZED.
     */
}
  cvd_cal_object_header_t;

typedef struct cvd_cal_table_object_t cvd_cal_table_object_t;

#include "apr_pack_begin.h"

struct cvd_cal_table_object_t
{
  cvd_cal_object_header_t header;
  uint32_t major_version;
  uint32_t minor_version;
  void* start_ptr;
  uint32_t size;
  uint32_t data_mem_handle;
  uint32_t num_columns;
  cvd_cal_column_descriptor_t* columns;
  uint8_t reorder_key_buf[ CVD_CAL_KEY_COLUMN_SIZE_IN_BYTES ];
  void* lut_start;
  uint32_t lut_size;
  void* cdft_start;
  uint32_t cdft_size;
  void* cdot_start;
  uint32_t cdot_size;
  void* data_start;
  uint32_t data_size;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_query_object_t cvd_cal_query_object_t;

#include "apr_pack_begin.h"

struct cvd_cal_query_object_t
{
  cvd_cal_object_header_t header;
  void* matching_entries_buffer;
  uint32_t matching_entries_buffer_size;
  uint32_t num_matching_entries;
  uint32_t table_handle;
}
#include "apr_pack_end.h"
;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE GENERIC CVD CAL OBJECT                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef union cvd_cal_object_t
{
  cvd_cal_object_header_t header;
  cvd_cal_table_object_t table;
  cvd_cal_query_object_t query;
}
  cvd_cal_object_t;

typedef struct cvd_cal_lut_work_item_t cvd_cal_lut_work_item_t;

#include "apr_pack_begin.h"

struct cvd_cal_lut_work_item_t{
  apr_list_node_t link;
  uint32_t lut_entry_index;
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

/** 
  Initialize the CVD calibration utility. Must be called before any other 
  calibration utility APIs can be called, and must be called only once.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_init (
  void
);


/** 
  Deinitialize the calibration utility.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_deinit (
  void
);


/**
  Evaluate the amount of book-keeping memory required, given a calibration
  table.

  @param[in] cal_table_descriptor  Calibration table descriptor.
  @param[out] ret_size             Required indexing memory size in bytes 
                                   for book-keeping.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_eval_indexing_mem_size (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t* ret_size
);


/**
  Parse a calibration table. If no book-keeping memory is provided (i.e.
  indexing_mem_addr == NULL), then linear look-up is performed. 

  @param[in] indexing_mem_addr  Indexing memory address.
  @param[in] indexing_mem_size  Indexing memory size in bytes.
  @param[in] table_descriptor   Calibration table descriptor.
  @param[out] ret_table_handle  CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_parse_table (
  void* indexing_mem_ptr,
  uint32_t indexing_mem_size,
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_handle_t** ret_table_handle
);


/**
  Query a parsed calibration table for a set of data entries matching the 
  provided key.

  @param[in] table_handle      CVD calibration table handle.
  @param[in] key               Query key.
  @param[in] entries_buf_size  User buffer size in bytes.
  @param[in/out] ret_entries   Pointer to user buffer.
  @param[out] ret_num_entries  Number of matched entries.

  @return APR_EOK when successful.
  @return APR_ENEEDMORE when user buffer is not bigger enough to store
                        all matched entries.
*/
APR_INTERNAL int32_t cvd_cal_query_table (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
);


/**
  Discard a calibration table when it has been de-registered with CVD.
  Also discard the book-keeping memory region for this table.

  @param[in] table_hanlde  CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_discard_table (
  cvd_cal_table_handle_t* table_hanlde
);


/**
  Find the min and max value for a specified column.

  @param[in] table_handle    CVD calibration table handle.
  @param[in] column_id       Column ID.
  @param[out] ret_min_value  Min column value.
  @param[out] ret_max_value  Max column value.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_find_min_max_column_value (
  cvd_cal_table_handle_t* table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
);

/**
  Find the min and max value for a specified column in compressed format table.

  @param[in] table_handle    CVD calibration table handle.
  @param[in] column_id       Column ID.
  @param[out] ret_min_value  Min column value.
  @param[out] ret_max_value  Max column value.

  @return APR_EOK when successful.
*/

APR_INTERNAL int32_t cvd_cal_find_min_max_column_value_v2 (
  uint32_t table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
);

/**
  Find the volume calibration format of the compressed calibration table.

  @param[in] table_handle    CVD calibration table handle.
  @param[out] bool_t         Boolean value
                               TRUE :- .V1 volume format
                               FALSE :- not a V1 volume format

  @return APR_EOK when successful.
*/

APR_INTERNAL int32_t cvd_cal_find_vol_cal_format (
  uint32_t table_handle,
  bool_t* ret_is_v1_vol_format
);

/**
  Parses a calibration table for subsequent use.

  @param[in] table_descriptor Calibration table descriptor.
  @param[out] ret_table_handle CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_parse_table_v2 (
  cvd_cal_table_descriptor_t* table_descriptor,
  uint32_t* ret_table_handle
);

/**
  Discards a calibration table.

  @param[in] table_handle CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_discard_table_v2 (
  uint32_t table_handle
);


/**
  This command initializes a calibration search with the help of query key.
  The matched entry details are saved in the provided matched_entry_buffer.
..A query handle is generated, to which the matched_entry_buffer is linked
    internally in cvd_cal utility. 
  Query handle is provided to the caller

  @param[in] table_handle CVD calibration table handle.
  @param[in] key Query key.
  @param[in] matched_entry_buffer. Buffer to save the matched entries.
  @param[in] matched_entry_buffer_size Size of the matched entry buffer.  
  @param[out] ret_cal_query_handle Calibration query handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query_init (
  uint32_t table_handle,
  cvd_cal_key_t* key,
  void* matched_entry_buffer,
  uint32_t matched_entry_buffer_size,
  uint32_t* ret_cal_query_handle
);

/**
  This command is used to retrieve calibration data from a valid cvd_cal_query_handle.
  This function returns after all the matching calibration entries have been provided 
  to the client via the client-supplied callback function (Multiple times as many required ).

  @param[in] cal_query_handle Calibration query handle.
  @param[in] cb_fn Client-supplied callback function.
  @param[in] cb_data Client-supplied data pointer for the callback function.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query (
  uint32_t cal_query_handle,
  cvd_cal_query_cb_fn_t cb_fn,
  const void* client_data
);

/**
  This command deinitializes a calibration query. all the references 
  linked to the query_handle are removed. 
  The Query Handle is made invalid after this command execution is completed.

  @param[in] cal_query_handle Calibration query handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query_deinit (
  uint32_t cal_query_handle
);

/**
  This command parses all matched entries of the query_handle 
  and removes the entries corresponding to NA values of a given column ID.

  @param[in] cal_query_handle Calibration query handle.
  @param[in] column_id Column ID from the Cal table.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_utility_remove_na_matching_entries(
  uint32_t query_handle,
  uint32_t column_id
);

/**
  Retrives the number of active apr objects used for CVD cal heap at that time of query.

  @param[out] objects_used - number of active objects 

  @return APR_EOK when successful.
*/
APR_INTERNAL uint32_t cvd_cal_obj_stat (
  uint32_t *objects_used
);

#endif /* __CVD_CAL_PRIVATE_I_H__ */