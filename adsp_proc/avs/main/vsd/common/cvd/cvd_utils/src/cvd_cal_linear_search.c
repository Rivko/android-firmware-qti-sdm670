/*
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_cal_linear_search.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <string.h>
#include "msg.h"
#include "mmstd.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "cvd_cal_private_i.h"
#include "cvd_cal_common_i.h"
#include "cvd_cal_linear_search_i.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

/*****************************************************************************
 * INTERNAL ROUTINES                                                           *
 ****************************************************************************/

/* Linear search column value comparison. */
static uint32_t cvd_cal_linear_search_cmp (
  void* key_addr,
  void* data_addr,
  cvd_cal_na_value_t na_val,
  uint32_t type_size
)
{
  /* Input parameters are verified in calling functions. */

  if ( ( memcmp( key_addr, data_addr, type_size ) != 0 )
      && ( memcmp( data_addr, &na_val, type_size ) != 0 ) )
  {
    return CVD_CAL_QUERY_NOT_MATCH;
  }

  return CVD_CAL_QUERY_MATCH;
}


/* Check if an entry in table is a match to query key. */
static uint32_t cvd_cal_linear_search_match_key (
  cvd_cal_table_descriptor_t* table_descriptor,
  uint8_t* data_ptr,
  cvd_cal_key_t* key,
  bool_t* is_matching
)
{
  int32_t rc = APR_EOK;
  uint32_t idx;
  uint32_t type_size;
  cvd_cal_column_descriptor_t* column_descriptor;
  cvd_cal_column_t* column;

  if ( ( table_descriptor == NULL ) ||
       ( data_ptr == NULL ) || ( key == NULL ) ||
       ( is_matching == NULL ) )
  {
    return APR_EBADPARAM;
  }

  column = key->columns;
  column_descriptor = table_descriptor->columns;
  *is_matching = TRUE;
  for ( idx = 0; idx < key->num_columns; ++idx )
  {
    type_size = cvd_cal_get_type_width( column_descriptor->type );
    if ( type_size == 0 )
    {
      rc = APR_EBADPARAM;
      break;
    }

    if ( cvd_cal_linear_search_cmp( &column->value, data_ptr,
                                    column_descriptor->na_value,
                                    type_size ) != CVD_CAL_QUERY_MATCH )
    {
       *is_matching = FALSE;
       rc = APR_EOK;
       break;
    }

    ++column_descriptor;
    data_ptr += type_size;
    ++column;
  }

  if ( ( rc == APR_EOK ) && ( *is_matching == TRUE ) )
  {
    *is_matching = TRUE;
  }

  return rc;
}

/* Linear search on a table. */
APR_INTERNAL int32_t cvd_cal_linear_search_query (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t buffer_left,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
)
{
  int32_t rc = APR_EOK;
  uint8_t* cur_row;
  uint8_t* table_end;
  cvd_cal_table_descriptor_t* table_descriptor;
  bool_t is_matching = FALSE;
  uint32_t row_size;
  uint32_t row_idx;
  uint32_t num_match_entry;
  cvd_cal_entry_t entry_item;
  const uint32_t item_size = sizeof( cvd_cal_entry_t );
  cvd_cal_key_t new_key;
  bool_t need_more = FALSE;

  if ( ( table_handle == NULL ) || ( key == NULL ) ||
       ( ret_entries == NULL ) || ( ret_num_entries == NULL ) )
  {
    return APR_EBADPARAM;
  }

  cur_row = ( ( uint8_t* )table_handle->table_descriptor.start_ptr );
  table_end = ( cur_row + table_handle->table_descriptor.size );
  *ret_num_entries = 0;

  row_idx = 0;
  num_match_entry = 0;
  table_descriptor = &table_handle->table_descriptor;

  /* Reorder columns in the query key to match the column order in calibration table. */
  ( void ) cvd_cal_reorder_key( table_descriptor, key, &new_key, table_handle->reorder_key_buf );

  /* Initial validation */
  rc = cvd_cal_validate_table( table_descriptor );
  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_linear_search_query(): Invalid table" );
    return rc;
  }

  while ( cur_row < table_end )
  {
    ++row_idx;
    row_size = cvd_cal_get_entry_info( table_handle, cur_row, &entry_item );

    /* Validate row size */
    rc = cvd_cal_validate_entry( table_descriptor, cur_row, row_size );
    if ( rc != APR_EOK )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_linear_search_query(): Invalid table" );
      break;
    }

    rc = cvd_cal_linear_search_match_key( table_descriptor, cur_row, &new_key, &is_matching );
    if ( rc != APR_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_linear_search_query(): Match key error, rc=%08X",
                                              rc );
      break;
    }

    if ( is_matching == TRUE )
    {
      ++num_match_entry;

      /* Copy matched entry to user buffer. */
      if ( item_size > buffer_left )
      {
        /* User buffer is too small. Stop copying but keep searching. */
        need_more = TRUE;
        cur_row += row_size;
        continue;
      }
      else
      {
        ( void ) mmstd_memcpy( ret_entries, buffer_left, &entry_item, item_size );
        buffer_left -= item_size;
        ++ret_entries;
      }
    }

    cur_row += row_size;
  }

  *ret_num_entries = num_match_entry;

  if ( ( rc == APR_EOK ) && ( need_more == TRUE ) )
  {
    rc = APR_ENEEDMORE;
  }
  /* else: APR_OK or APR_EFAILED or
           return rc from cvd_cal_linear_search_match_key(). */

  return rc;
}

/* Linear search on a compressed table. */
APR_INTERNAL int32_t cvd_cal_linear_search_query_v2 (
  cvd_cal_table_object_t* table_object,
  cvd_cal_key_t* key,
  uint8_t* matched_entries_buffer,
  uint32_t matched_entries_buffer_size,
  uint32_t* ret_num_entries
)
{
  int32_t rc;
  cvd_cal_key_t new_key;
  cvd_cal_table_descriptor_t table_descriptor;
  uint8_t* lut_entry_ptr;
  uint32_t num_lut_entries;
  uint32_t lut_entry_size;
  uint8_t* lut_end;
  bool_t is_matching = FALSE;
  uint32_t num_match_entry;
  uint32_t* matching_lut_array;
  uint32_t buffer_left;
  uint32_t lut_entry_index = 0;
  bool_t need_more = FALSE;

  if ( ( table_object == NULL ) || ( key == NULL ) || 
       ( matched_entries_buffer == NULL ) || ( ret_num_entries == NULL ) )
  {
    return APR_EBADPARAM;
  }

  table_descriptor.start_ptr = table_object->start_ptr;
  table_descriptor.size = table_object->size;
  table_descriptor.data_mem_handle = table_object->data_mem_handle;
  table_descriptor.num_columns = table_object->num_columns;
  table_descriptor.columns = table_object->columns;
  table_descriptor.indexing_mem_handle = 0;

  /* Reorder columns in the query key to match the column order in calibration table. */
  ( void ) cvd_cal_reorder_key( &table_descriptor, key, &new_key, 
                                table_object->reorder_key_buf );

  /* Validate the table. */
    rc = cvd_cal_validate_table( &table_descriptor );
  if ( rc != APR_EOK )
  {
    MSG ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_linear_search_query_v2(): Invalid table" );
    return rc;
  }

  lut_entry_size =  ( ( table_descriptor.num_columns * 
                        cvd_cal_get_type_width( table_descriptor.columns->type )
                      ) + ( sizeof ( uint32_t ) * 2 )
                    );

  num_match_entry = 0;
  matching_lut_array = ( uint32_t* ) matched_entries_buffer;
  buffer_left = matched_entries_buffer_size;
  
  lut_entry_ptr = ( uint8_t* ) table_object->lut_start;
  lut_end = lut_entry_ptr + table_object->lut_size;

  num_lut_entries = *( ( uint32_t* )lut_entry_ptr );

  lut_entry_ptr += sizeof( uint32_t );

  while ( ( lut_entry_ptr < lut_end ) && ( lut_entry_index < num_lut_entries ) )
  {
    rc = cvd_cal_linear_search_match_key( &table_descriptor, lut_entry_ptr, 
                                          &new_key, &is_matching );
    if ( rc != APR_EOK )
    {
       MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_linear_search_query_v2():"
               "Match key error, rc=%08X", rc );
      break;
    }

    if ( is_matching == TRUE )
    {
      /* Copy matched entry to user buffer. */
      if ( buffer_left < sizeof( uint32_t ) )
      {
        /* User buffer is too small. Stop copying but keep searching. */
        need_more = TRUE;
        lut_entry_ptr += lut_entry_size;
        lut_entry_index++;
        continue;
      }
      else
      {
        matching_lut_array[ num_match_entry ] = lut_entry_index;
        buffer_left -= sizeof( uint32_t );
      }
      
      ++num_match_entry;
    }

    lut_entry_ptr += lut_entry_size;
    lut_entry_index++;
  }

  *ret_num_entries = num_match_entry;

  if ( ( rc == APR_EOK ) && ( need_more == TRUE ) )
  {
    rc = APR_ENEEDMORE;
  }
  /* else: APR_OK or APR_EFAILED or 
           return rc from cvd_cal_linear_search_match_key(). */

  return APR_EOK;
}
