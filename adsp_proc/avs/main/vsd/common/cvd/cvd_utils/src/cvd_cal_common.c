/*
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_cal_common.c#1 $
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
#include "cvd_cal_private_i.h"
#include "cvd_cal_common_i.h"
#include "cvd_mem_mapper_i.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Required data alignment for calibration data */
#define CVD_CAL_DATA_ALIGNMENT ( 4 )

/* Required data alignment for indexing */
#define CVD_CAL_INDEXING_ALIGNMENT ( 32 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

/*****************************************************************************
 * COMMON ROUTINES                                                             *
 ****************************************************************************/

/* Get width in bytes of a data type. */
APR_INTERNAL uint32_t cvd_cal_get_type_width (
  uint32_t data_type
)
{
  uint32_t width_in_bytes;

  switch ( data_type )
  {
#if 0
    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8:
      width_in_bytes = 1;
      break;
    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16:
      width_in_bytes = 2;
      break;
#endif
    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32:
      width_in_bytes = 4;
      break;
#if 0
    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64:
      width_in_bytes = 8;
      break;
#endif

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_get_type_width(): Invalid data type 0x%08X",
                                              data_type );
      width_in_bytes = 0;
      break;
  }

  return width_in_bytes;
}


/* Get the size/data pair information from an entry in table,
   return the total size of this entry. */
APR_INTERNAL uint32_t cvd_cal_get_entry_info (
  cvd_cal_table_handle_t* table_handle,
  uint8_t* cur_row,
  cvd_cal_entry_t* entry_item
)
{
  uint8_t* data_ptr;
  uint32_t data_size;

  /* Assume input parameters are verified in calling function. */

  data_ptr = ( cur_row + table_handle->meta_size );
  data_size = ( *( ( uint32_t* ) data_ptr ) );/* Data size is uint32_t */

  entry_item->size = data_size;
  entry_item->start_ptr = ( ( void* )( data_ptr + sizeof( uint32_t ) ) );

  return ( table_handle->meta_size + sizeof( uint32_t ) + data_size );
}


/* Get the meta size (size excluding the actual data and data size)
   of a table entry. */
APR_INTERNAL int32_t cvd_cal_get_entry_meta_size (
  cvd_cal_column_descriptor_t* column_descriptor,
  uint32_t num_columns,
  uint32_t* ret_size
)
{
  uint32_t idx;
  uint32_t meta_size = 0;
  uint32_t type_size;

  if ( ( column_descriptor == NULL ) || ( ret_size == NULL ) )
  {
    return APR_EBADPARAM;
  }

  for ( idx = 0; idx < num_columns; ++idx )
  {
    type_size = cvd_cal_get_type_width( ( ( cvd_cal_column_descriptor_t* )column_descriptor )->type );
    meta_size += type_size;
    ++column_descriptor;
  }

  *ret_size = meta_size;

  return APR_EOK;
}


/* Get number of cal entries in from a cal table. */
APR_INTERNAL int32_t cvd_cal_get_num_cal_entries (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t* ret_num_cal_entries
)
{
  int32_t rc = APR_EOK;
  uint8_t* cur_row;
  uint8_t* table_end;
  uint32_t num_cal_entries;
  uint32_t row_size;
  uint32_t meta_size;
  uint32_t data_size;

  if ( ret_num_cal_entries == NULL )
  {
    return APR_EBADPARAM;
  }

  cur_row = ( ( uint8_t* )cal_table_descriptor->start_ptr );
  table_end = ( cur_row + cal_table_descriptor->size );
  num_cal_entries = 0;

  /* Validate table */
  rc = cvd_cal_validate_table( cal_table_descriptor );
  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_get_num_cal_entries(): Invalid table" );
    return APR_EFAILED;
  }

  ( void ) cvd_cal_get_entry_meta_size ( cal_table_descriptor->columns, cal_table_descriptor->num_columns, &meta_size );

  /* meta_size should always be smaller than the table itself */
  if( meta_size > cal_table_descriptor->size )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_get_num_cal_entries(): Invalid table" );
    return APR_EFAILED;
  }

  while( cur_row < table_end )
  {
    data_size = ( *( ( uint32_t* )( cur_row + meta_size ) ) ) ;
    row_size = ( meta_size + sizeof( uint32_t ) + data_size );

    rc = cvd_cal_validate_entry( cal_table_descriptor, cur_row, row_size );
    if ( rc != APR_EOK )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_get_num_cal_entries(): Invalid table" );
      break;
    }

    cur_row += row_size;
    ++num_cal_entries;
  }

  *ret_num_cal_entries = num_cal_entries;

  return rc;
}


/* Form a new key by re-ordering a query key.
   Columns in new key have the same order as in calibration table.
   N/A values are filled into new key if the original query
   key does not have all the columns present. */
APR_INTERNAL int32_t cvd_cal_reorder_key (
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_key_t* orig_key,
  cvd_cal_key_t* ret_key,
  uint8_t* column_buf
 )
{
  uint32_t table_col;
  uint32_t key_col;
  cvd_cal_column_t* key_buf;
  cvd_cal_column_t* key_column;
  cvd_cal_column_descriptor_t* table_column;
  uint32_t found_key_column;

  if ( ( table_descriptor == NULL ) || ( orig_key == NULL ) ||
       ( ret_key == NULL ) || ( column_buf == NULL ) )
  {
    return APR_EBADPARAM;
  }

  key_buf = ( cvd_cal_column_t* )column_buf;
  ret_key->num_columns = table_descriptor->num_columns;
  ret_key->columns = key_buf;
  table_column = table_descriptor->columns;
  for ( table_col = 0; table_col < table_descriptor->num_columns; ++table_col )
  {
    key_buf->id = table_column->id;
    key_column = orig_key->columns;
    found_key_column = 0;
    for ( key_col = 0; key_col < orig_key->num_columns; ++key_col )
    {
      /* Take care of out of order key. */
      if ( table_column->id == key_column->id )
      {
        key_buf->value = key_column->value;
        found_key_column = 1;
        break;
      }
      ++key_column;
    }

    /* Take care of incomplete key. */
    if ( ( table_col < table_descriptor->num_columns ) &&
         ( found_key_column == 0 ) )
    {
      /* Currently supported max column width is 32 bits. */
      key_buf->value = table_column->na_value.uint32_val;
    }

    ++table_column;
    ++key_buf;
  }

  return APR_EOK;
}

/* Validate the boundaries and alignment of the calibration table */
APR_INTERNAL int32_t cvd_cal_validate_table(
  cvd_cal_table_descriptor_t* table_descriptor
)
{
  int32_t rc = APR_EOK;
  /* Validate mem handle */
  rc = cvd_mem_mapper_validate_handle(table_descriptor->data_mem_handle);
  if ( APR_EOK != rc)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_table(): Invalid mem map handle" );
    return rc;
  }

  /* Validate address and size */
  rc = cvd_mem_mapper_validate_virt_mem_is_in_region( table_descriptor->data_mem_handle,
                                                      table_descriptor->start_ptr,
                                                      table_descriptor->size );
  if ( APR_EOK != rc)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_table(): Invalid table address" );
    return rc;
  }

  return rc;
}


/* Validates that the current mem map handle is valid, that the data size
   lies within the mapped region, and that pointers are aligned to 4 byte boundaries */
APR_INTERNAL int32_t cvd_cal_validate_entry(
  cvd_cal_table_descriptor_t* table_descriptor,
  void* entry_ptr,
  uint32_t data_size
)
{
  int32_t rc = APR_EOK;
  int8_t *data_begin, *data_end;
  int8_t *table_begin, *table_end;
  /* Validate table*/
  rc = cvd_cal_validate_table(table_descriptor);
  if ( APR_EOK != rc)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_entry(): Invalid table" );
    return rc;
  }

  data_begin = (int8_t*)entry_ptr;
  data_end = data_begin + data_size;

  table_begin = (int8_t*)table_descriptor->start_ptr;
  table_end = table_begin + table_descriptor->size;

  /* Validate that address to address+size is within table bounds
     If it is within table bounds, it is also automatically within mapped
     region bounds */
  if((data_begin < table_begin) ||
     (data_begin > table_end) ||
     (data_end > table_end) ||
     /* check below required because size can be large enough to cause data_end to wrap around
        and still potentially be in the correct range */
     (data_size > table_descriptor->size)
     )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_entry(): Invalid address/size 0x%x, %u",
                                            (uint32_t)entry_ptr, data_size );
    return APR_EBADPARAM;
  }

  /* Verify alignment */
  rc = cvd_mem_mapper_validate_access_align( entry_ptr, data_size, CVD_CAL_DATA_ALIGNMENT );
  if ( APR_EOK != rc)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_entry(): Invalid alignment" );
    return rc;
  }

  /* Everything is good */
  return APR_EOK;
}

/* Validates that the indexing mem map handle is valid, that the size
   lies within the mapped region, and that pointers are aligned to 4 byte boundaries */
APR_INTERNAL int32_t cvd_cal_validate_indexing_mem(
  cvd_cal_table_descriptor_t* table_descriptor,
  void* indexing_ptr,
  uint32_t indexing_size
)
{
  int32_t rc = APR_EOK;
  /* Validate mem handle */
  rc = cvd_mem_mapper_validate_handle( table_descriptor->indexing_mem_handle );
  if ( APR_EOK != rc)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_indexing_mem(): Invalid indexing handle" );
    return rc;
  }

  rc = cvd_mem_mapper_validate_virt_mem_is_in_region( table_descriptor->indexing_mem_handle,
                                                      indexing_ptr,
                                                      indexing_size );
  if ( APR_EOK != rc)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_indexing_mem(): Invalid indexing address 0x%x, size %u",
                                            (uint32_t)indexing_ptr,
                                            indexing_size );
    return rc;
  }

  /* Validate that address is aligned
     This is already done outside, can be removed or kept for extra safety */
  rc = cvd_mem_mapper_validate_access_align( indexing_ptr, indexing_size, CVD_CAL_INDEXING_ALIGNMENT );
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_indexing_mem(): Invalid alignment of address/size 0x%x, %u",
                                            (uint32_t)indexing_ptr,
                                            indexing_size );
    return APR_EBADPARAM;
  }

  return APR_EOK;
}
