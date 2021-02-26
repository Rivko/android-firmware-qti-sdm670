/*
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_cal.c#2 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"
#include "mmstd.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_lock.h"
#include "vss_common_public_if.h"
#include "cvd_cal_private_i.h"
#include "cvd_cal_common_i.h"
#include "cvd_cal_linear_search_i.h"
#include "cvd_mem_mapper_i.h"
#include "adsp_vparams_api.h"
#include "vss_params_public_if.h"
#include "cvd_cal_log_i.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Max 48 tables ( 16 CVS sessions + 16 * 2 CVP sessions ) */
#define CVD_CAL_HEAP_SIZE ( 10000 )

#define CVD_CAL_HANDLE_TOTAL_BITS_V ( 16 )
#define CVD_CAL_HANDLE_INDEX_BITS_V ( 6 ) /* 6 bits = 64 handles. */

#define CVD_CAL_MAX_OBJECTS_V ( 1 << CVD_CAL_HANDLE_INDEX_BITS_V )

/* Required data alignment for calibration data */
#ifndef CVD_CAL_DATA_ALIGNMENT
  #define CVD_CAL_DATA_ALIGNMENT ( 4 )
#endif
#define CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ( 4 )

#define CVD_CAL_DATA_FORMAT_MAJOR_VERSION_V1 ( 0x00000001 )
#define CVD_CAL_DATA_FORMAT_MINOR_VERSION_V0 ( 0x00000000 )
#define CVD_CAL_DATA_FORMAT_MINOR_VERSION_V1 ( 0x00000001 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

static apr_lock_t cvd_cal_int_lock;

/* Heap Management. */
static uint8_t cvd_cal_heap_pool[ CVD_CAL_HEAP_SIZE ];
apr_memmgr_type cvd_cal_heapmgr;

/* Object Management */
static apr_objmgr_object_t cvd_cal_object_table[ CVD_CAL_MAX_OBJECTS_V ];
static apr_objmgr_t cvd_cal_objmgr;

/****************************************************************************
 * OBJECT CREATION AND DESTRUCTION ROUTINES                                 *
 ****************************************************************************/

static int32_t cvd_cal_mem_alloc_object (
  uint32_t size,
  cvd_cal_object_t** ret_object
)
{
  int32_t rc;
  cvd_cal_object_t* obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the new CVD CAL object. */
    obj =  ( cvd_cal_object_t* ) apr_memmgr_malloc( &cvd_cal_heapmgr, size );
    if ( obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_mem_alloc_object():"
             "Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the CVD CAL object. */
    rc = apr_objmgr_alloc_object( &cvd_cal_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_mem_alloc_object():"
             " Out of objects, result: 0x%08X", rc );
      apr_memmgr_free( &cvd_cal_heapmgr, obj );
      return APR_ENORESOURCE;
    }

    /* Link the CVD CAL object to the handle. */
    objmgr_obj->any.ptr = obj;

    /* Initialize allocated object with null values. */
    ( void ) mmstd_memset( obj, 0xFD, size );

    /* Initialize the base CVP object header. */
    obj->header.handle = objmgr_obj->handle;
    obj->header.type = CVD_CAL_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }
  *ret_object = obj;

  return APR_EOK;
}

static int32_t cvd_cal_free_object (
  cvd_cal_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( object->header.type )
  {
  case CVD_CAL_OBJECT_TYPE_ENUM_TABLE:
    {
      cvd_cal_table_object_t* table_object = ( cvd_cal_table_object_t* ) object;
      apr_memmgr_free( &cvd_cal_heapmgr, table_object->columns );
    }
    break;

  case CVD_CAL_OBJECT_TYPE_ENUM_QUERY:
    break;

  default:
    break;

  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &cvd_cal_objmgr, object->header.handle );
  apr_memmgr_free( &cvd_cal_heapmgr, object );

  return APR_EOK;
}

static int32_t cvd_cal_create_table_object (
  cvd_cal_table_object_t** ret_obj
)
{
  int32_t rc;
  cvd_cal_table_object_t* obj;

  if ( ret_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cvd_cal_mem_alloc_object( sizeof( cvd_cal_table_object_t ),
                                 ( ( cvd_cal_object_t** ) &obj ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_create_table_object():"
           "Failed to allocate object, result: 0x%08X", rc );
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    obj->header.type = CVD_CAL_OBJECT_TYPE_ENUM_TABLE;
    obj->major_version = 0xFFFFFFFF;
    obj->minor_version = 0xFFFFFFFF;
    obj->start_ptr = NULL;
    obj->lut_start = NULL;
    obj->cdft_start = NULL;
    obj->cdot_start = NULL;
    obj->data_start = NULL;
    obj->columns = NULL;
    obj->size = 0;
    obj->lut_size = 0;
    obj->cdft_size = 0;
    obj->cdot_size = 0;
    obj->data_size = 0;
    obj->num_columns = 0;
  }

  *ret_obj = obj;

  return APR_EOK;
}

static int32_t cvd_cal_create_query_object (
  cvd_cal_query_object_t** ret_obj
)
{
  int32_t rc;
  cvd_cal_query_object_t* obj;

  if ( ret_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = cvd_cal_mem_alloc_object( sizeof( cvd_cal_query_object_t ),
                                 ( ( cvd_cal_object_t** ) &obj ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_create_query_object():"
           " Failed to allocate object, result: 0x%08X", rc );
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    obj->header.type = CVD_CAL_OBJECT_TYPE_ENUM_QUERY;
    obj->matching_entries_buffer = NULL;
    obj->matching_entries_buffer_size = 0;
    obj->num_matching_entries = 0;
    obj->table_handle = ( uint32_t ) NULL;
  }

  *ret_obj = obj;

  return APR_EOK;
}

static int32_t cvd_cal_typecast_object (
  apr_objmgr_object_t* store,
  cvd_cal_object_type_enum_t type,
  cvd_cal_object_t** ret_obj
)
{
  cvd_cal_object_t* obj;

  if ( ( store == NULL ) || ( ret_obj == NULL ) )
  {
    return APR_EBADPARAM;
  }

  obj = ( ( cvd_cal_object_t* ) store->any.ptr );

  if ( ( obj == NULL ) || ( obj->header.type != type ) )
  {
    return APR_EFAILED;
  }

  *ret_obj = obj;

  return APR_EOK;
}

static int32_t cvd_cal_get_typed_object (
  uint32_t handle,
  cvd_cal_object_type_enum_t type,
  cvd_cal_object_t** ret_obj
)
{
  int32_t rc;
  apr_objmgr_object_t* store;

  if ( handle == 0 )
  {
    return APR_EHANDLE;
  }

  rc = apr_objmgr_find_object( &cvd_cal_objmgr, handle, &store );
  if ( rc ) return rc;

  rc = cvd_cal_typecast_object( store, type, ret_obj );
  if ( rc ) return rc;

  return APR_EOK;
}

/****************************************************************************
 * HELPER/API ROUTINES                                                          *
 ****************************************************************************/

static void cvd_cal_int_lock_fn ( void )
{
  ( void ) apr_lock_enter( cvd_cal_int_lock );
}

static void cvd_cal_int_unlock_fn ( void )
{
  ( void ) apr_lock_leave( cvd_cal_int_lock );
}

/* Get offset (in bytes with respect to entry start) and data type
   and N/A value of a specified column. */
static int32_t cvd_cal_get_column_offset_and_info (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t column_id,
  uint32_t* ret_offset,
  uint32_t* ret_type,
  uint32_t* na_value
)
{
  int32_t rc;
  uint32_t idx;
  uint32_t offset = 0;
  cvd_cal_column_descriptor_t* column;
  bool_t is_found = FALSE;

  if ( ( cal_table_descriptor == NULL ) || ( ret_offset == NULL ) ||
       ( ret_type == NULL ) )
  {
    return APR_EBADPARAM;
  }

  column = cal_table_descriptor->columns;

  for ( idx = 0; idx < cal_table_descriptor->num_columns; ++idx )
  {
    if ( column_id == column->id )
    {
      /* Found matching column ID. */
      is_found = TRUE;
      break;
    }

    offset += cvd_cal_get_type_width( column->type );
    ++column;
  }

  if ( is_found == TRUE )
  {
    *ret_offset = offset;
    *ret_type = column->type;
    *na_value = column->na_value.uint32_val;
    rc = APR_EOK;
  }
  else
  {
    /* Matching column ID not found. */
    rc = APR_EFAILED;
  }

  return rc;
}


/* Get the column value. */
static int32_t cvd_cal_get_column_value (
  uint8_t* data_ptr,
  uint32_t column_data_type,
  uint32_t* ret_column_value
)
{
  if ( ( data_ptr == NULL ) || ( ret_column_value == NULL ) )
  {
    return APR_EBADPARAM;
  }

  switch ( column_data_type )
  {
    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8:
      *ret_column_value = ( *( uint8_t* )data_ptr );
      break;

    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16:
      *ret_column_value = ( *( uint16_t* )data_ptr );
      break;

    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32:
      *ret_column_value = ( *( uint32_t* )data_ptr );
      break;

    case VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64:
      /* Currently supported max column value is 32-bit. */
      /* Fall through. */

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cvd_cal_get_column_value(): Invalid column data type 0x%08X",
                                              column_data_type );
      return APR_EBADPARAM;
  }

  return APR_EOK;
}

/* Allocate a table handle from internal heap. */
static int32_t cvd_cal_get_table_handle(
  cvd_cal_table_handle_t** ret_table_handle
)
{
  int32_t rc;
  cvd_cal_table_handle_t* handle;

  if ( ret_table_handle == NULL )
  {
    return APR_EBADPARAM;
  }

  handle = ( cvd_cal_table_handle_t* )apr_memmgr_malloc( &cvd_cal_heapmgr,
                                                         sizeof( cvd_cal_table_handle_t ) );
  if ( handle == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cvd_cal_get_table_handle(): no memory for table handle, size %d",
                                            sizeof( cvd_cal_table_handle_t ) );
    rc = APR_ENORESOURCE;
  }
  else
  {
    mmstd_memset( handle, 0, sizeof( cvd_cal_table_handle_t ) );
    handle->is_parsed = FALSE;
    handle->aux_info = NULL;

    *ret_table_handle = handle;
    rc = APR_EOK;
  }

  return rc;
}


/* Free table handle back to internal heap. */
static int32_t cvd_cal_free_table_handle(
  cvd_cal_table_handle_t* table_handle
)
{
  apr_memmgr_free( &cvd_cal_heapmgr, table_handle->table_descriptor.columns );

  if ( table_handle->aux_info != NULL )
  {
    apr_memmgr_free( &cvd_cal_heapmgr, table_handle->aux_info );
  }

  apr_memmgr_free( &cvd_cal_heapmgr, table_handle );

  return APR_EOK;
}

static int32_t cvd_cal_validate_column_type(
  uint32_t type_size
)
{
  /* Presently only allow 4 byte columns */
  if( type_size != 4 )
  {
    return APR_EFAILED;
  }
  return APR_EOK;
}

/* Copy the column info from table descriptor to
   table handle. */
static int32_t cvd_cal_copy_column_info(
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_handle_t* table_handle
)
{
  int32_t rc;
  uint32_t column_idx;
  uint32_t type_size;
  uint32_t src_column_size;
  uint8_t* src_column;
  uint32_t dst_size;
  cvd_cal_column_descriptor_t* dst_column;

  if ( ( table_descriptor == NULL ) || ( table_handle == NULL ) )
  {
    return APR_EBADPARAM;
  }

  dst_size = table_descriptor->num_columns * sizeof( cvd_cal_column_descriptor_t );

  dst_column = ( cvd_cal_column_descriptor_t* )apr_memmgr_malloc( &cvd_cal_heapmgr,
                                                                  dst_size );
  if ( dst_column == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_copy_column_info(): no memory for columns, size %d",
                                            dst_size );
    rc = APR_ENORESOURCE;
  }
  else
  {
    table_handle->table_descriptor.columns = dst_column;

    src_column = ( uint8_t* )table_descriptor->columns;
    for ( column_idx = 0; column_idx < table_descriptor->num_columns; ++column_idx )
    {
      /* Currently ACDB uses 32-bit for column value, while CVD internally
         supports up to 64-bit. Take care of the difference here.
         Assumption: the max data width supported by CVD will NOT be smaller
         than that of client. */
      type_size = cvd_cal_get_type_width( ( ( cvd_cal_column_descriptor_t* )src_column )->type );

      /* Disallow sizes other than 32 bits for now. */
      rc = cvd_cal_validate_column_type( type_size );
      if( rc != APR_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_copy_column_info(): disallowed width %u",
                                                type_size );
        break;
      }
      src_column_size = ( sizeof( cvd_cal_column_descriptor_t ) -
                          sizeof( cvd_cal_na_value_t ) + type_size );
      if ( dst_size < src_column_size )
      {
        rc = APR_ENOMEMORY;
        break;
      }
      /* Assumption: little-endian platform. */
      ( void ) mmstd_memcpy( dst_column, dst_size, src_column, src_column_size );

      dst_column++;
      src_column += src_column_size;
      dst_size -= src_column_size;
    }
    rc = APR_EOK;
  }

  return rc;
}

/* Copy the column info from table descriptor to
   table object. */
static int32_t cvd_cal_copy_column_info_v2 (
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_object_t* table_object
)
{
  int32_t rc;
  uint32_t column_idx;
  uint32_t type_size;
  uint32_t src_column_size;
  uint8_t* src_column;
  uint32_t dst_size;
  cvd_cal_column_descriptor_t* dst_column;

  if ( ( table_descriptor == NULL ) || ( table_object == NULL ) )
  {
    return APR_EBADPARAM;
  }

  dst_size = ( table_descriptor->num_columns ) * sizeof( cvd_cal_column_descriptor_t );

  dst_column = ( cvd_cal_column_descriptor_t* ) apr_memmgr_malloc( &cvd_cal_heapmgr,
                                                                    dst_size );
  if ( dst_column == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_copy_column_info_v2():"
           " no memory for columns, size %d", dst_size );
    rc = APR_ENORESOURCE;
  }
  else
  {
    table_object->columns = dst_column;

    src_column = ( uint8_t* ) table_descriptor->columns;
    for ( column_idx = 0; column_idx < table_descriptor->num_columns; ++column_idx )
    {
      /* Currently ACDB uses 32-bit for column value, while CVD internally
         supports up to 64-bit. Take care of the difference here.
         Assumption: the max data width supported by CVD will NOT be smaller
         than that of client. */
      type_size = cvd_cal_get_type_width( ( ( cvd_cal_column_descriptor_t* ) src_column )->type );

      /* Disallow sizes other than 32 bits for now. */
      rc = cvd_cal_validate_column_type( type_size );
      if( rc != APR_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_copy_column_info_v2():"
               " disallowed width %u", type_size );
        break;
      }
      src_column_size = ( sizeof( cvd_cal_column_descriptor_t ) -
                          sizeof( cvd_cal_na_value_t ) + type_size );
      if ( dst_size < src_column_size )
      {
        rc = APR_ENOMEMORY;
        break;
      }
      /* Assumption: little-endian platform. */
      ( void ) mmstd_memcpy( dst_column, dst_size, src_column, src_column_size );

      dst_column++;
      src_column += src_column_size;
      dst_size -= src_column_size;
    }
    rc = APR_EOK;
  }

  return rc;
}


/* Parse a calibration table. If no book-keeping memory is provided (i.e.
   indexim_mem_addr == NULL, then linear look-up is performed. */
APR_INTERNAL int32_t cvd_cal_parse_table (
  void* indexing_mem_ptr,
  uint32_t indexing_mem_size,
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_handle_t** ret_table_handle
)
{
  int32_t rc;
  cvd_cal_table_handle_t* table_handle = NULL;
  uint32_t checkpoint = 0;

  uint32_t meta_size = 0;
  /* Note: Taking address of packed member 'meta_size' of class or structure
   'cvd_cal_table_handle_t' may result in an unaligned pointer value
   [-Werror,-Waddress-of-packed-member], hence local variable is intruduced 
   to avoid this error. */

  for ( ;; )
  {
    if ( ( table_descriptor == NULL ) || ( ret_table_handle == NULL ) )
    {
      rc = APR_EBADPARAM;
      break;
    }
    checkpoint = 1;

    rc = cvd_cal_get_table_handle( &table_handle );
    if ( (rc != APR_EOK ) || (!table_handle) )
    {
      break;
    }
    checkpoint = 2;

    /* Fill in info in table handle. */
    {
      ( void ) mmstd_memcpy( &table_handle->table_descriptor,
                             sizeof( table_handle->table_descriptor ),
                             table_descriptor,
                             sizeof( cvd_cal_table_descriptor_t ) );

      rc = cvd_cal_copy_column_info( table_descriptor, table_handle );
      if ( rc != APR_EOK )
      {
        break;
      }

      ( void ) cvd_cal_get_entry_meta_size( table_handle->table_descriptor.columns,
                                            table_handle->table_descriptor.num_columns, &meta_size );
      table_handle->meta_size = meta_size;

      checkpoint = 3;
    }

    if ( indexing_mem_ptr == NULL )
    {
      table_handle->is_parsed = FALSE;
      *ret_table_handle = table_handle;
      return APR_EOK;
    }
    else
    {
#if 0
      rc = cvd_cal_nonlinear_search_parse_table( indexing_mem_ptr,
                                                 indexing_mem_size,
                                                 table_handle );
      if ( rc == APR_EOK )
      {
        *ret_table_handle = table_handle;
        return APR_EOK;
      }
      else
      {
        break;
      }
#endif 
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_table(): non-linear search not supported" );
    }
    checkpoint = 4;
  }

  switch ( checkpoint )
  {
  case 4:
    /*-fallthru */
  case 3:
    apr_memmgr_free( &cvd_cal_heapmgr, table_handle->table_descriptor.columns );
    /*-fallthru */
  case 2:
    if ( table_handle->aux_info != NULL )
    {
      apr_memmgr_free( &cvd_cal_heapmgr, table_handle->aux_info );
    }
    apr_memmgr_free( &cvd_cal_heapmgr, table_handle );
    /*-fallthru */
  case 1:
    /*-fallthru */
  default:
    break;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_parse_table(): failed at checkpoint = %d, rc = %d",
                                          checkpoint, rc );

  return rc;
}


/* Query a parsed calibration for a set of data entries matching the provided
   key. */
APR_INTERNAL int32_t cvd_cal_query_table (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
)
{
  int32_t rc;

  for ( ;; )
  {
    if ( ( table_handle == NULL ) || ( key == NULL ) ||
       ( ret_entries == NULL ) || ( ret_num_entries == NULL ) )
    {
      rc = APR_EBADPARAM;
      break;
    }

    if ( key->num_columns > table_handle->table_descriptor.num_columns )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_table(): Invalid key, key num_columns %d, table num_columns %d",
                                              key->num_columns,
                                              table_handle->table_descriptor.num_columns );
      rc = APR_EBADPARAM;
      break;
    }

    if ( table_handle->is_parsed == FALSE )
    {
      /* Linear search. */
      rc = cvd_cal_linear_search_query( table_handle, key, entries_buf_size,
                                        ret_entries, ret_num_entries );
      break;
    }
    else
    {
#if 0
      /* Nonlinear search. */
      rc = cvd_cal_nonlinear_search_query( table_handle, key, entries_buf_size,
                                           ret_entries, ret_num_entries );
      break;
#endif
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_table(): non-linear search not supported" );
    }
  }

  return rc;
}


/* Discard a calibration table when it has been de-registered with CVD.
   Also discard the book-keeping memory region for this table. */
APR_INTERNAL int32_t cvd_cal_discard_table (
  cvd_cal_table_handle_t* table_hanlde
)
{
  ( void ) cvd_cal_free_table_handle( table_hanlde );

  return APR_EOK;
}

/* Get offset (in bytes with respect to LUT entry start) and data type
   and N/A value of a specified column. */
static int32_t cvd_cal_get_column_offset_and_info_v2 (
  cvd_cal_table_object_t* table_object,
  uint32_t column_id,
  uint32_t* ret_offset,
  uint32_t* ret_type,
  uint32_t* na_value
)
{
  int32_t rc;
  uint32_t idx;
  uint32_t offset = 0;
  cvd_cal_column_descriptor_t* column;
  bool_t is_found = FALSE;

  if ( ( table_object == NULL ) || ( ret_offset == NULL ) ||
       ( ret_type == NULL ) || ( na_value == NULL ) )
  {
    return APR_EBADPARAM;
  }

  column = table_object->columns;

  for ( idx = 0; idx < table_object->num_columns; ++idx )
  {
    if ( column_id == column->id )
    {
      /* Found matching column ID. */
      is_found = TRUE;
      break;
    }

    offset += cvd_cal_get_type_width( column->type );
    ++column;
  }

  if ( is_found == TRUE )
  {
    *ret_offset = offset;
    *ret_type = column->type;
    *na_value = column->na_value.uint32_val;
    rc = APR_EOK;
  }
  else
  {
    /* Matching column ID not found. */
    rc = APR_EFAILED;
  }

  return rc;
}

APR_INTERNAL int32_t cvd_cal_get_minor_version (
  uint32_t handle,
  uint32_t* minor_version
)
{
  int32_t rc = APR_EOK;
  cvd_cal_table_object_t* table_object;
  cvd_cal_query_object_t* query_object; 
  for ( ;; )
  {
    rc = cvd_cal_get_typed_object( handle, CVD_CAL_OBJECT_TYPE_ENUM_QUERY ,
                                    ( cvd_cal_object_t** ) &query_object );
    if ( APR_EOK == rc )
    {
       handle = query_object->table_handle;
    }
    rc = cvd_cal_get_typed_object( handle, CVD_CAL_OBJECT_TYPE_ENUM_TABLE ,
                                    ( cvd_cal_object_t** ) &table_object );

    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_get_minor_version():"
           "Invalid handle" );
      rc = APR_EBADPARAM;
      break;
    }
    *minor_version = table_object->minor_version;
    break;
  }
  return rc;
}

APR_INTERNAL int32_t cvd_cal_find_min_max_column_value_v2 (
  uint32_t table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
)
{
  int32_t rc;
  uint32_t min_value = 0XFFFFFFFF;
  uint32_t max_value = 0;
  cvd_cal_table_object_t* table_object;
  uint8_t* lut_entry;
  uint32_t num_lut_entries;
  uint32_t lut_entry_size;
  uint32_t index = 0;
  uint32_t column_offset;
  uint32_t column_data_type;
  uint32_t column_value;
  uint32_t na_value; /* Currently all column values are 32-bit. */

  for ( ;; )
  {
    if ( ( ret_min_value == NULL ) || ( ret_max_value == NULL ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value_v2():"
           "Invalid input parameters" );
      rc = APR_EBADPARAM;
      break;
    }

    /* validate table handle. */
    rc = cvd_cal_get_typed_object( table_handle, CVD_CAL_OBJECT_TYPE_ENUM_TABLE ,
                                    ( cvd_cal_object_t** ) &table_object );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value_v2():"
           "Invalid table_handle" );
      rc = APR_EBADPARAM;
      break;
    }

    lut_entry_size =  ( ( table_object->num_columns *
                          cvd_cal_get_type_width( table_object->columns->type )
                        ) + ( sizeof( uint32_t ) * 2 )
                      );

    lut_entry = ( uint8_t* ) table_object->lut_start;

    num_lut_entries = *( ( uint32_t* ) lut_entry );
    lut_entry += sizeof( uint32_t );

    rc = cvd_cal_get_column_offset_and_info_v2( table_object, column_id,
                                                &column_offset, &column_data_type,
                                                &na_value );
    if ( rc != APR_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value_v2():"
             "No matching column ID 0x%08X found", column_id );
      rc = APR_EBADPARAM;
      break;
    }

    while ( index < num_lut_entries )
    {
      rc = cvd_cal_get_column_value( ( lut_entry + column_offset ),
                                     column_data_type, &column_value );
      if ( rc != APR_EOK )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_find_min_max_column_value_v2():"
               "cannot get column value for column id %d, lut_entry %d",
               column_id, index );
        continue;
      }

      /* Currently all column values are 32-bit. */
      if ( ( column_value != na_value ) && ( column_value < min_value ) )
      {
        min_value = column_value;
      }

      /* Currently all column values are 32-bit. */
      if ( ( column_value != na_value ) && ( column_value > max_value ) )
      {
        max_value = column_value;
      }

      lut_entry += lut_entry_size;
      index++;
    }

    *ret_min_value = min_value;
    *ret_max_value = max_value;

    break;
  }

  return rc;
}

APR_INTERNAL int32_t cvd_cal_find_vol_cal_format (
  uint32_t table_handle,
  bool_t* ret_is_v1_vol_format
)
{
  int32_t rc;
  cvd_cal_table_object_t* table_object;
  bool_t is_vol_module_found = FALSE;
  uint8_t* cdft_index_ptr;
  uint32_t size_parsed = 0;
  uint32_t num_mid_pid_pairs;
  uint32_t entry_index = 0;
  uint32_t module_id;
  uint32_t param_id;
  uint32_t minor_version = 0;
  uint32_t is_instance_id_supported = 0;
  rc = cvd_cal_get_minor_version( table_handle, &minor_version );
  if ( rc )
  {
    return APR_EBADPARAM;
  }
  is_instance_id_supported = ( ( minor_version > 0 )? 1:0 );

  #define MID_PID_PAIR_BYTE_SIZE ( ( sizeof( uint32_t ) * 2 ) + ( is_instance_id_supported * sizeof( uint32_t ) ) )
  #define CDFT_ENTRY_SIZE ( ( MID_PID_PAIR_BYTE_SIZE * num_mid_pid_pairs ) +\
                             sizeof( uint32_t ) )

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_find_vol_cal_format():"
         "minor_version %d",minor_version );
  if ( ret_is_v1_vol_format == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_vol_cal_format():"
         " Invalid input parameters" );
    return APR_EBADPARAM;
  }
  rc = cvd_cal_get_typed_object( table_handle, CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
                                 ( cvd_cal_object_t** ) &table_object );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_vol_cal_format():"
         " Invalid table_handle" );
    return APR_EBADPARAM;
  }
  *ret_is_v1_vol_format = FALSE;

  cdft_index_ptr = ( uint8_t* ) table_object->cdft_start;

  while ( size_parsed < table_object->cdft_size )
  {
    num_mid_pid_pairs = *( ( uint32_t* ) cdft_index_ptr );
    cdft_index_ptr += sizeof( uint32_t );

    while ( entry_index < num_mid_pid_pairs )
    {
      module_id = *( ( uint32_t* ) cdft_index_ptr );
	  
	  /* Account for the instance id parameter. */
      cdft_index_ptr += sizeof( uint32_t ) + ( is_instance_id_supported * sizeof( uint32_t ) );
	  
      param_id = *( ( uint32_t* ) cdft_index_ptr );
      cdft_index_ptr += sizeof( uint32_t );

      if ( ( module_id == VOICE_MODULE_RX_VOL ) &&
           ( param_id == VOICE_PARAM_VOL ) )
      {
        is_vol_module_found = TRUE;
        *ret_is_v1_vol_format = TRUE;
        rc = APR_EOK;
        break;
      }
      else if ( ( module_id == VSS_MODULE_RX_VOLUME ) &&
                ( param_id == VSS_PARAM_VOLUME ) )
      {
        is_vol_module_found = TRUE;
        *ret_is_v1_vol_format = FALSE;
        rc = APR_EOK;
        break;
      }
      entry_index++;
    }
    size_parsed += CDFT_ENTRY_SIZE;
    entry_index = 0;

    if ( is_vol_module_found == TRUE )
    {
      break;
    }
  }

  if ( is_vol_module_found == FALSE )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_vol_cal_format():"
         "No format info found" );
    rc = APR_EFAILED;
  }

#undef CDFT_ENTRY_SIZE
#undef MID_PID_PAIR_BYTE_SIZE

  return rc;
}

/* Find the min and max value for a specified column. */
/* NOTE: revisit when ACDB uses more than 32-bit data type. */
APR_INTERNAL int32_t cvd_cal_find_min_max_column_value (
  cvd_cal_table_handle_t* table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
)
{
  int32_t rc;
  uint32_t min_value = 0XFFFFFFFF;
  uint32_t max_value = 0;
  uint8_t* cur_row;
  uint8_t* table_end;
  uint32_t row_size;
  cvd_cal_entry_t entry_item;
  uint32_t column_offset;
  uint32_t column_data_type;
  uint32_t column_value;
  uint32_t na_value; /* Currently all column values are 32-bit. */

  for ( ;; )
  {
    if ( ( table_handle == NULL ) ||
         ( ret_min_value == NULL ) || ( ret_max_value == NULL ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value(): Invalid input parameters" );
      rc = APR_EBADPARAM;
      break;
    }

    cur_row = ( uint8_t* )table_handle->table_descriptor.start_ptr;
    table_end = cur_row + table_handle->table_descriptor.size;

    rc = cvd_cal_validate_table( &table_handle->table_descriptor );
    if ( rc != APR_EOK )
    {
      return rc;
    }
    rc = cvd_cal_get_column_offset_and_info( &table_handle->table_descriptor,
                                             column_id, &column_offset, &column_data_type, &na_value );
    if ( rc != APR_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value(): No matching column ID 0x%08X found",
                                              column_id );
      rc = APR_EBADPARAM;
      break;
    }

    while ( cur_row < table_end )
    {
      row_size = cvd_cal_get_entry_info( table_handle, cur_row, &entry_item );

      rc = cvd_cal_validate_entry( &table_handle->table_descriptor, cur_row, row_size );
      if ( rc != APR_EOK )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value(): Invalid table" );
        break;
      }

      /* Column offset should always be smaller than row_size */
      if( column_offset > row_size )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_find_min_max_column_value(): Invalid table" );
        rc = APR_EFAILED;
        break;
      }

      rc = cvd_cal_get_column_value( ( cur_row + column_offset ), column_data_type, &column_value );
      if ( rc != APR_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_find_min_max_column_value(): cannot get column value for column id %d",
                                               column_id );
        continue;
      }

      /* Currently all column values are 32-bit. */
      if ( ( column_value != na_value ) && ( column_value < min_value ) )
      {
        min_value = column_value;
      }

      /* Currently all column values are 32-bit. */
      if ( ( column_value != na_value ) && ( column_value > max_value ) )
      {
        max_value = column_value;
      }

      cur_row += row_size;
    }

    *ret_min_value = min_value;
    *ret_max_value = max_value;

    break;
  }

  return rc;
}

APR_INTERNAL int32_t cvd_cal_validate_version_info (
  cvd_cal_table_descriptor_t* table_descriptor,
  uint32_t* major_version,
  uint32_t* minor_version
)
{
  uint8_t* table_index_ptr;
  int rc = APR_EOK;

  /* Verify alignment */
  rc = cvd_mem_mapper_validate_access_align( table_descriptor->start_ptr,
         table_descriptor->size, CVD_CAL_DATA_ALIGNMENT );
  if ( rc != APR_EOK )
  {
    return APR_EBADPARAM;
  }

  table_index_ptr = ( uint8_t* ) table_descriptor->start_ptr;

  *major_version = *( ( uint32_t* ) table_index_ptr );
  table_index_ptr += sizeof( uint32_t );

  *minor_version = *( ( uint32_t* ) table_index_ptr );

  switch ( *major_version )
  {
  case CVD_CAL_DATA_FORMAT_MAJOR_VERSION_V1:
    {
      switch ( *minor_version )
      {
      case CVD_CAL_DATA_FORMAT_MINOR_VERSION_V0:
        break;
      case CVD_CAL_DATA_FORMAT_MINOR_VERSION_V1:
        break;

      default:
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_version_info():"
               " Unsupported versions Major version = %d Minor versions = %d",
               ( *major_version ), ( *minor_version ) );
        return APR_EBADPARAM;
      }
    }
    break;

  default:
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_version_info():"
           " Unsupported versions Major version = %d Minor versions = %d",
           ( *major_version ), ( *minor_version ) );
    return APR_EBADPARAM;
  }

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_cal_validate_column_info (
  uint32_t num_columns,
  uint8_t* column_buffer
)
{
  cvd_cal_column_descriptor_t* columns;
  uint32_t column_buf_index = 0;
  uint32_t column_index = 0;
  uint32_t type_size;
  int rc = APR_EBADPARAM;

  if ( column_buffer == NULL )
  {
    return APR_EBADPARAM;
  }

  while ( column_index < num_columns )
  {
    columns = ( cvd_cal_column_descriptor_t* )( column_buffer + column_buf_index );
    switch ( columns->id )
    {
    case VSS_ICOMMON_CAL_COLUMN_NETWORK:
    case VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE:
    case VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE:
    case VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX:
    case VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE:
    case VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE:
    case VSS_ICOMMON_CAL_COLUMN_MEDIA_ID:
    case VSS_ICOMMON_CAL_COLUMN_FEATURE:
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_column_info():"
             " Unsupported column ID = %d", columns->id );
      return APR_EBADPARAM;
    }

    type_size = cvd_cal_get_type_width( columns->type );

    /* Disallow sizes other than 32 bits for now. */
    rc = cvd_cal_validate_column_type( type_size );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_column_info():"
             " Unsupported column type = %d", columns->type );
      return rc;
    }

    column_buf_index += ( sizeof( cvd_cal_column_descriptor_t ) -
                          sizeof( cvd_cal_na_value_t ) + type_size );
    column_index++;

  }

  return rc;
}

APR_INTERNAL int32_t cvd_cal_validate_lut_info (
  cvd_cal_table_descriptor_t* table_descriptor,
  void** lut_start,
  uint32_t* lut_size
 )
{
  uint8_t* table_start;
  uint32_t table_offset = 0;
  uint32_t block_size;
  uint32_t num_lut_entries;
  uint32_t lut_entry_size;
  uint32_t cdft_size;
  uint32_t cdot_size;
  uint32_t cdft_offset;
  uint32_t cdot_offset;
  uint32_t i = 0;
  #define TABLE_VERSION_INFO_BYTE_SIZE ( 2 * sizeof( uint32_t ) )
  #define LUT_ENTRY_CDFTOFFSET_BYTE_OFFSET \
            ( table_descriptor->num_columns * \
              cvd_cal_get_type_width( table_descriptor->columns->type ) )

  lut_entry_size =  ( ( table_descriptor->num_columns *
                        cvd_cal_get_type_width( table_descriptor->columns->type )
                      ) + ( sizeof ( uint32_t ) * 2 )
                    );

  table_start = ( uint8_t* ) table_descriptor->start_ptr;

  table_offset += ( TABLE_VERSION_INFO_BYTE_SIZE );

  /* Verify if the LUT block size is valid. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  /* Verify alignment */
  if ( block_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_lut_info():"
           " Misaligned LUT size = %d at table_offset = %d", block_size,
           table_offset );
    return APR_EBADPARAM;
  }

  if ( block_size >= table_descriptor->size )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_lut_info():"
           " Invalid LUT size = %d for table_size = %d", block_size,
           table_descriptor->size );
    return APR_EBADPARAM;
  }

  *lut_size = block_size;

  table_offset += CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE;
  *lut_start = ( void* ) ( table_start + table_offset );
  num_lut_entries = *( ( uint32_t* )( table_start + table_offset ) );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_validate_lut_info():"
         "LUT size = %d num_LUT_Entries = %d", *lut_size, num_lut_entries );

  /* Compute LUT size from lut entry info. */
  block_size = ( ( num_lut_entries * lut_entry_size ) + sizeof ( uint32_t ) );

  if ( ( *lut_size ) != block_size )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_lut_info():"
           " Invalid LUT size = %d for num_lut_entries = %d", ( *lut_size ),
           num_lut_entries );
    return APR_EBADPARAM;
  }

  /* Validate CDFT and CDOT offsets in the LUT entries */
  {
    /* Align table_offset to CDFT size field. */
    table_offset = TABLE_VERSION_INFO_BYTE_SIZE +
                     CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE + ( *lut_size );
    cdft_size = *( ( uint32_t* )( table_start + table_offset ) );

    /* Align table_offset to CDOT size field. */
    table_offset += ( CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE + cdft_size );
    cdot_size = *( ( uint32_t* )( table_start + table_offset ) );

    for ( i = 0; i < num_lut_entries; i++ )
    {
      /* Align table_offset to the CDFT offset field of current LUT entry */ 
      table_offset = ( TABLE_VERSION_INFO_BYTE_SIZE + sizeof( uint32_t ) +
                       sizeof( uint32_t ) + ( i * lut_entry_size ) +
                       LUT_ENTRY_CDFTOFFSET_BYTE_OFFSET );
      cdft_offset = *( ( uint32_t* )( table_start + table_offset ) );

      table_offset += sizeof( uint32_t );
      cdot_offset = *( ( uint32_t* )( table_start + table_offset ) );

      if ( ( cdft_offset >= cdft_size ) || ( cdot_offset >= cdot_size ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_lut_info():"
               " Invalid offset(s) cdft_offset = %d, cdot_offset = %d, table_offset = %d",
               cdft_offset, cdot_offset, table_offset );
        return APR_EBADPARAM;
      }
    }
  }

  #undef LUT_ENTRY_CDFTOFFSET_BYTE_OFFSET
  #undef TABLE_VERSION_INFO_BYTE_SIZE

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_cal_validate_cdft_info (
  cvd_cal_table_descriptor_t* table_descriptor,
  void** cdft_start,
  uint32_t* cdft_size,
  uint32_t minor_version
 )
{
  uint8_t* table_start;
  uint32_t table_offset = 0;
  uint32_t block_size;
  uint32_t num_mid_pid_pairs;
  uint32_t cdft_entry_size;
  uint32_t remaining_table_size;
  uint32_t is_instance_id_supported;

  is_instance_id_supported = ( ( minor_version > 0 )? 1:0 );
  #define MID_PID_PAIR_BYTE_SIZE ( ( sizeof( uint32_t ) * 2 ) + ( is_instance_id_supported * sizeof( uint32_t ) ) )
  #define TABLE_VERSION_INFO_BYTE_SIZE ( 2 * sizeof( uint32_t ) )

  remaining_table_size = table_descriptor->size;
  table_start = ( uint8_t* ) table_descriptor->start_ptr;

  table_offset += ( TABLE_VERSION_INFO_BYTE_SIZE );
  remaining_table_size = remaining_table_size - TABLE_VERSION_INFO_BYTE_SIZE;

  /* Get LUT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

  /* Get CDFT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  /* Verify alignment */
  if ( block_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdft_info():"
           " Misaligned CDFT size = %d at table_offset = %d", block_size,
           table_offset );
    return APR_EBADPARAM;
  }

  if( block_size > ( remaining_table_size - CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdft_info():"
           " CDFT size = %d goes out of bound. remaining_table_size = %d",
           block_size, remaining_table_size );
    return APR_EBADPARAM;
  }

  *cdft_size = block_size;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_validate_cdft_info():"
         "CDFT size = %d table_offset = %d", *cdft_size, table_offset );

  table_offset += CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE;

  *cdft_start = ( void* )( table_start + table_offset );

  /* Start validating CDFT block. */
  block_size = 0;
  do
  {
    num_mid_pid_pairs = *( ( uint32_t* )( table_start + table_offset ) );

    cdft_entry_size = ( MID_PID_PAIR_BYTE_SIZE * num_mid_pid_pairs );
    block_size += sizeof( uint32_t ) + cdft_entry_size;
    table_offset += sizeof( uint32_t ) + cdft_entry_size;
  }
  while( block_size < ( *cdft_size ) );

  if ( ( block_size != ( *cdft_size ) ) || ( block_size >= remaining_table_size ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdft_info():"
           " Invalid CDFT size = %d for the cdft content size = %d", ( *cdft_size ),
           block_size );
    return APR_EBADPARAM;
  }

  #undef TABLE_VERSION_INFO_BYTE_SIZE
  #undef MID_PID_PAIR_BYTE_SIZE

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_cal_validate_cdot_info (
  cvd_cal_table_descriptor_t* table_descriptor,
  void** cdot_start,
  uint32_t* cdot_size
 )
{
  uint8_t* table_start;
  uint32_t table_offset = 0;
  uint32_t block_size;
  uint32_t num_data_offset_entries;
  uint32_t cdot_entry_size;
  uint32_t remaining_table_size;
  uint32_t temp_remaining_table_size;
  uint32_t data_table_size;
  uint32_t data_offset;
  uint32_t i = 0;

  #define DATA_OFFSET_ENTRY_BYTE_SIZE ( sizeof( uint32_t ) )
  #define TABLE_VERSION_INFO_BYTE_SIZE ( 2 * sizeof( uint32_t ) )

  remaining_table_size = table_descriptor->size;
  table_start = ( uint8_t* ) table_descriptor->start_ptr;

  table_offset += ( TABLE_VERSION_INFO_BYTE_SIZE );
  
  if ( remaining_table_size < TABLE_VERSION_INFO_BYTE_SIZE )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
           " Corrupted table descriptor size = %d", table_descriptor->size );
    return APR_EBADPARAM;    
  }
  
  remaining_table_size = remaining_table_size - TABLE_VERSION_INFO_BYTE_SIZE;

  /* Get LUT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

  /* Get CDFT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

  /* Get CDOT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  /* Verify alignment */
  if ( block_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
           " Misaligned CDOT size = %d at table_offset = %d", block_size,
           table_offset );
    return APR_EBADPARAM;
  }

  if( block_size > ( remaining_table_size - CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
           " CDOT size = %d goes out of bound. remaining_table_size = %d",
           block_size, remaining_table_size );
    return APR_EBADPARAM;
  }

  data_table_size = *( ( uint32_t* )( table_start + table_offset +
                                      CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE + block_size ) );

  temp_remaining_table_size = remaining_table_size - ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
 
  /* Verify alignment */
  if ( data_table_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
           " Misaligned data_table_size size = %d at table_offset = %d", data_table_size,
            ( table_offset + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE + block_size ) );
    return APR_EBADPARAM;
  }

  if( data_table_size > ( temp_remaining_table_size - CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
           " data_table_size size = %d goes out of bound. remaining_table_size = %d",
           data_table_size, temp_remaining_table_size );
    return APR_EBADPARAM;
  }

  *cdot_size = block_size;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_validate_cdot_info():"
         "CDOT size = %d table_offset = %d", *cdot_size, table_offset );

  table_offset += CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE;

  *cdot_start = ( void* )( table_start + table_offset );

  /* Start validating CDOT block size. */
  block_size = 0;
  do
  {
    num_data_offset_entries = *( ( uint32_t* )( table_start + table_offset ) );
    cdot_entry_size = ( DATA_OFFSET_ENTRY_BYTE_SIZE * num_data_offset_entries );
    /* Start validating CDOT entries. */
    for ( i = 0; i < num_data_offset_entries; i++ )
    {
      table_offset += DATA_OFFSET_ENTRY_BYTE_SIZE;
      data_offset = *( ( uint32_t* )( table_start + table_offset ) );
      if ( data_offset >= data_table_size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdot_info():"
               " Invalid data_offset entry = %d at table_offset = %d",
               data_offset, table_offset );
        return APR_EBADPARAM;
      }
    }
    block_size += sizeof ( uint32_t ) + cdot_entry_size;
    table_offset += sizeof ( uint32_t );
  }
  while( block_size < ( *cdot_size ) );

  if ( ( block_size != ( *cdot_size ) ) || ( block_size >= remaining_table_size ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_cdft_info():"
           " Invalid CDFT size = %d for the content size = %d",
           ( *cdot_size ), block_size );
    return APR_EBADPARAM;
  }

  #undef TABLE_VERSION_INFO_BYTE_SIZE
  #undef DATA_OFFSET_ENTRY_BYTE_SIZE

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_cal_validate_data_info (
  cvd_cal_table_descriptor_t* table_descriptor,
  void** data_start,
  uint32_t* data_size
 )
{
  uint8_t* table_start;
  uint32_t table_offset = 0;
  uint32_t block_size;
  uint32_t remaining_table_size;
  uint32_t data_entry_size;
  #define TABLE_VERSION_INFO_BYTE_SIZE ( 2 * sizeof( uint32_t ) )

  remaining_table_size = table_descriptor->size;
  table_start = ( uint8_t* ) table_descriptor->start_ptr;

  table_offset += ( TABLE_VERSION_INFO_BYTE_SIZE );
  remaining_table_size = remaining_table_size - TABLE_VERSION_INFO_BYTE_SIZE;

  /* Get LUT size */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

  /* Get CDFT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

  /* Get CDOT size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  remaining_table_size -= ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );
  table_offset += ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE );

/* Get Data Table size. */
  block_size = *( ( uint32_t* )( table_start + table_offset ) );

  /* Verify alignment */
  if ( block_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_data_info():"
           " Misaligned DATA table size = %d at table_offset = %d", block_size,
           table_offset );
    return APR_EBADPARAM;
  }

  if( block_size > ( remaining_table_size - CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_data_info():"
           " DATA TABLE size = %d goes out of bound. remaining_table_size = %d",
           block_size, remaining_table_size );
    return APR_EBADPARAM;
  }

  *data_size = block_size;
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_validate_data_info():"
         "DATA TABLE size = %d table_offset = %d", *data_size, table_offset );

  table_offset += CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE;

  *data_start = ( void* )( table_start + table_offset );

  /* Start validating DATA block. */
  block_size = 0;

  do
  {
    data_entry_size = *( ( uint32_t* )( table_start + table_offset ) );
    
  /* Verify alignment */
  if ( data_entry_size & ( 0x00000003 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_data_info():"
           " Misaligned dat_entry_size = %d at table_offset = %d", data_entry_size,
           table_offset );
    return APR_EBADPARAM;
  }
    block_size += sizeof( uint32_t ) + data_entry_size;
    table_offset += sizeof( uint32_t ) + data_entry_size;
  }
  while( block_size < ( *data_size ) );

  if ( ( block_size != ( *data_size ) ) || ( ( block_size + CVD_CAL_TABLE_SIZE_FIELD_BYTE_SIZE ) != remaining_table_size ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_validate_data_info():"
           " Invalid DATA TABLE size = %d for the content size = %d",
           ( *data_size ), block_size );
    return APR_EBADPARAM;
  }

  #undef TABLE_VERSION_INFO_BYTE_SIZE
  return APR_EOK;
}

/**
  This utility API needs to be used with compressed calibration table.
  Parses and validates the integrity of the calibration table for subsequent use.

  @param[in] table_descriptor Calibration table descriptor.
  @param[out] ret_table_handle CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_parse_table_v2 (
  cvd_cal_table_descriptor_t* table_descriptor,
  uint32_t* ret_table_handle
)
{
  int32_t rc;
  uint32_t checkpoint = 0;
  uint32_t major_version;
  uint32_t minor_version;
  void* lut_start;
  uint32_t lut_size;
  void* cdft_start;
  uint32_t cdft_size;
  void* cdot_start;
  uint32_t cdot_size;
  void* data_start;
  uint32_t data_size;
  cvd_cal_table_object_t* table_object;

  for ( ;; )
  {
    if ( ( table_descriptor == NULL ) || ( ret_table_handle == NULL ) )
    {
      rc = APR_EBADPARAM;
      break;
    }
    checkpoint = 1;

    rc =  cvd_cal_validate_version_info( table_descriptor, &major_version,
                                          &minor_version );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 2;

    rc = cvd_cal_validate_column_info( table_descriptor->num_columns,
                                        ( uint8_t* ) table_descriptor->columns );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 3;

    rc = cvd_cal_validate_lut_info( table_descriptor, &lut_start, &lut_size );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 4;

    rc = cvd_cal_validate_cdft_info( table_descriptor, &cdft_start, &cdft_size, minor_version );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 5;

    rc = cvd_cal_validate_cdot_info( table_descriptor, &cdot_start, &cdot_size );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 6;

    rc = cvd_cal_validate_data_info( table_descriptor, &data_start, &data_size );
    if ( rc != APR_EOK )
    {
      break;
    }
    checkpoint = 7;

    /* create cvd_cal_table_object and obtain a handle. */
    rc = cvd_cal_create_table_object( &table_object );
    if ( rc )
    {
      break;
    }
    checkpoint = 8;

    /* Fill in info in table object. */
    {
      table_object->major_version = major_version;
      table_object->minor_version = minor_version;
      table_object->start_ptr = table_descriptor->start_ptr;
      table_object->size = table_descriptor->size;
      table_object->data_mem_handle = table_descriptor->data_mem_handle;
      table_object->num_columns = table_descriptor->num_columns;
      table_object->lut_start = lut_start;
      table_object->lut_size = lut_size;
      table_object->cdft_start = cdft_start;
      table_object->cdft_size = cdft_size;
      table_object->cdot_start = cdot_start;
      table_object->cdot_size = cdot_size;
      table_object->data_start = data_start;
      table_object->data_size = data_size;

      rc = cvd_cal_copy_column_info_v2( table_descriptor, table_object );
      if ( rc != APR_EOK )
      {
        break;
      }
    }

    *ret_table_handle = table_object->header.handle;
    return APR_EOK;

  }

  switch ( checkpoint )
  {
  case 8:
    cvd_cal_free_object( ( cvd_cal_object_t* )table_object );
    /*-fallthru */
  case 7:
    /*-fallthru */
  case 6:
    /*-fallthru */
  case 5:
    /*-fallthru */
  case 4:
    /*-fallthru */
  case 3:
    /*-fallthru */
  case 2:
    /*-fallthru */
  case 1:
    /*-fallthru */
  default:
    break;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_parse_table_v2():"
         " failed at checkpoint = %d, rc = %d", checkpoint, rc );

  return rc;

}


/**
  Discards a calibration table.

  @param[in] table_handle CVD calibration table handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_discard_table_v2 (
  uint32_t table_handle
)
{
  int32_t rc;
  cvd_cal_object_t* ret_obj;

  rc = cvd_cal_get_typed_object( table_handle, CVD_CAL_OBJECT_TYPE_ENUM_TABLE, &ret_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_discard_table_v2():"
           "table not found = %d", rc );
    return rc;
  }

  rc = cvd_cal_free_object( ret_obj );

  return rc;
}

/**
 * Reorders the matching entries with in matching_list by applying 
 * NA overwrite rule.
 */
APR_INTERNAL int32_t cvd_cal_apply_overwrite_na_rule (
  cvd_cal_table_object_t* table_object,
  cvd_cal_key_t* key,
  uint32_t* matching_list,
  int16_t low_index,
  int16_t high_index,
  uint32_t column_index
)
{
  uint32_t lut_entry_index;
  cvd_cal_column_t* key_column;
  uint8_t* column_index_ptr;
  uint32_t lut_entry_size;
  uint32_t table_column_value;
  uint32_t entry_offset;
  uint32_t column_offset;
  int16_t indexNaEnd = high_index; /* Marks end of NA entry. */
  int16_t indexNonNaStart = low_index; /* Marks start of non NA entry. */


  lut_entry_size = ( ( sizeof( uint32_t ) * 2 ) +
                     ( table_object->num_columns *
                       cvd_cal_get_type_width( table_object->columns->type ) ) );

  column_offset = column_index * cvd_cal_get_type_width( table_object->columns->type );

  /* Align the index to the first column of first LUT entry. */
  column_index_ptr = ( uint8_t* )table_object->lut_start + sizeof( uint32_t );
  /* Align for required column of first LUT entry.*/
  column_index_ptr += column_offset;

  key_column = &( key->columns[ column_index ] );

  /* Separate NA and non-NA entries into two Sub Arrays. */
  while ( indexNonNaStart <= indexNaEnd )
  {
    lut_entry_index = matching_list[indexNonNaStart];

    entry_offset = lut_entry_size * lut_entry_index;
    table_column_value = *( ( uint32_t* )( column_index_ptr + entry_offset ) );

    /* Algorithm requires for a particular column index in Calibration key
     * all lut_entry_index with NA value shall fall on left part of macthing 
     * list and lut_entry_index with column value same as key to fall on right 
     * side of the matching list. */
    if ( table_column_value != key_column->value )
    {
      indexNonNaStart++; /* Increment mark for NON-NA start by 1. */
    }
    else
    {
      /* Move non-NA value to indexNaEnd. */
      matching_list[indexNonNaStart] = matching_list[indexNaEnd];
      matching_list[indexNaEnd] = lut_entry_index;
      indexNaEnd--;
    }
  }

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "cvd_cal_apply_overwrite_na_rule(): "
         "low_index=(%d), indexNaEnd=(%d) - indexNonNaStart=(%d) high_index=(%d)",
         low_index, indexNaEnd, indexNonNaStart, high_index );

  /* This Sub-array has NA entry corresponding to column_index.
   * Re-curse same Algorithm only if sub array has more than 1 element.
   * This also serves as the exit Criteria for resursion.
   */
  if( ( column_index != 0 ) && ( indexNaEnd > low_index ) )
  {
    cvd_cal_apply_overwrite_na_rule( table_object, key, matching_list,
                                     low_index, indexNaEnd, ( column_index -1 ) );
  }

  /* This Sub-array has Non-NA entry corresponding to column_index.
   * Re-curse same Algorithm only if sub array has more than 1 element.
   * This also serves as the exit Criteria for resursion.
   */
  if( ( column_index != 0 ) && ( high_index > indexNonNaStart ) )
  {  
    cvd_cal_apply_overwrite_na_rule( table_object, key, matching_list,
                                     indexNonNaStart, high_index, ( column_index -1 ) );
  }

  return APR_EOK;
}

/**
  Initializes a calibration query.

  @param[in] table_handle CVD calibration table handle.
  @param[in] key Query key.
  @param[out] ret_cal_query_handle Calibration query handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query_init (
  uint32_t table_handle,
  cvd_cal_key_t* key,
  void* matched_entry_buffer,
  uint32_t matched_entry_buffer_size,
  uint32_t* ret_cal_query_handle

)
{
  int32_t rc;
  cvd_cal_table_object_t* table_object;
  uint32_t num_matching_entries;
  cvd_cal_key_t reordered_key;
  cvd_cal_query_object_t* query_object;
  cvd_cal_log_commit_info_t log_info;
  cvd_cal_log_system_config_header_t log_info_system_config;
  uint8_t* temp_log_buffer = NULL;
  uint32_t temp_log_buf_size = 0;
  int16_t low_index = 0;
  int16_t high_index = 0;

  for ( ; ; )
  {
    if ( ( key == NULL ) || ( matched_entry_buffer == NULL ) || 
         ( matched_entry_buffer_size == ( uint32_t ) NULL ) )
    {
      rc = APR_EBADPARAM;
      break;
    }

    rc = cvd_cal_get_typed_object( table_handle, CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
                                   ( cvd_cal_object_t** ) &table_object );
    if ( rc )
    {
      MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_init():"
             "invalid table handle = %d", table_handle );
      break;
    }

    if ( key->num_columns > table_object->num_columns )
    {
      MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_init():"
              "Invalid key, key num_columns %d, table num_columns %d",
              key->num_columns, table_object->num_columns );
      rc = APR_EBADPARAM;
      break;
    }

    rc = cvd_cal_linear_search_query_v2( table_object, key,
                                         ( uint8_t* ) matched_entry_buffer,
                                         matched_entry_buffer_size,
                                         &num_matching_entries );
    if ( rc )
    {
      break;
    }

    reordered_key.num_columns = table_object->num_columns;
    reordered_key.columns = ( cvd_cal_column_t* )table_object->reorder_key_buf;

    /* Apply Specific NA rule. */
    high_index = num_matching_entries - 1;
    cvd_cal_apply_overwrite_na_rule( table_object, &reordered_key,
                                     ( uint32_t* ) matched_entry_buffer,
                                     low_index, high_index,
                                     ( reordered_key.num_columns - 1 ) );

    /* create query object */
    rc = cvd_cal_create_query_object ( &query_object );
    if ( rc )
    {
      return rc;
    }

    /* Save matched entry info in Query object */
    query_object->matching_entries_buffer = matched_entry_buffer;
    query_object->matching_entries_buffer_size = matched_entry_buffer_size;
    query_object->num_matching_entries = num_matching_entries;
    query_object->table_handle = table_handle;

    *ret_cal_query_handle = query_object->header.handle;

     MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_query_init():"
             "Query Handle %d, number of matching entries %d",
             ( *ret_cal_query_handle ), num_matching_entries );

    /* Log query key. */
    {
      temp_log_buf_size = sizeof( uint32_t ) + ( key->num_columns * sizeof( cvd_cal_column_t ) );
      temp_log_buffer = ( uint8_t* )apr_memmgr_malloc( &cvd_cal_heapmgr, temp_log_buf_size );
      if ( temp_log_buffer != NULL )
      {
        *( ( uint32_t* )temp_log_buffer ) = key->num_columns;

        ( void ) mmstd_memcpy( ( void* )( ( uint8_t* )temp_log_buffer + sizeof( uint32_t ) ),
                               ( temp_log_buf_size - sizeof( uint32_t ) ),
                               ( void* )key->columns, ( temp_log_buf_size - sizeof( uint32_t ) )
                             );

        log_info_system_config.table_handle = table_handle;
        log_info_system_config.cal_query_handle = *ret_cal_query_handle;

        log_info.instance = 0;
        log_info.call_num = 0;
        log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_SYSTEM_CONFIG;
        log_info.data_container_header_size = sizeof ( log_info_system_config );
        log_info.data_container_header = &log_info_system_config;
        log_info.payload_size = temp_log_buf_size;
        log_info.payload_buf = temp_log_buffer;

        cvd_cal_log_data ( ( log_code_type ) LOG_ADSP_CVD_CAL_DATA_C,
                            CVD_CAL_LOG_SYSTEM_CONFIG,
                            ( void* )&log_info, sizeof( log_info ) );

        apr_memmgr_free( &cvd_cal_heapmgr, ( void* )temp_log_buffer );
      }
    }

    break;
  }

  return rc;
}


/**
  Deinitializes a calibration query.
  The given cal_query_handle will be invalid after deinit().

  @param[in] cal_query_handle Calibration query handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query_deinit (
  uint32_t cal_query_handle
)
{
  cvd_cal_query_object_t* query_object;
  int32_t rc;

  rc = cvd_cal_get_typed_object( cal_query_handle, CVD_CAL_OBJECT_TYPE_ENUM_QUERY,
                                 ( cvd_cal_object_t** ) &query_object );

  if ( rc )
  {
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query_deinit():"
            "invalid query handle = %d", cal_query_handle );
    return rc;
  }

  ( void ) cvd_cal_free_object( (cvd_cal_object_t*) query_object );

  return APR_EOK;
}


/**
  Queries the calibration table. This function returns after all the matching
  calibration entries have been provided to the client via the client-supplied
  callback function.

  @param[in] cal_query_handle Calibration query handle.
  @param[in] cb_fn Client-supplied callback function.
  @param[in] cb_data Client-supplied data pointer for the callback function.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_cal_query (
  uint32_t cal_query_handle,
  cvd_cal_query_cb_fn_t cb_fn,
  const void* client_data
)
{
  int32_t rc = APR_EOK;
  cvd_cal_query_object_t* query_object;
  cvd_cal_table_object_t* table_object;
  uint8_t* lut_entry;
  uint32_t lut_entry_size;
  uint32_t cdft_offset;
  uint32_t cdot_offset;
  uint32_t data_offset;
  uint32_t* matching_entries_array;
  uint32_t i=0;
  uint32_t lut_offset;
  uint8_t* lut_entry_start;
  uint32_t num_mid_pid_pairs;
  uint32_t num_data_offset_entries;
  uint32_t* cdft_index_ptr;
  uint32_t* cdot_index_ptr;
  uint32_t* data_index_ptr;
  uint32_t cal_param_count = 0;
  uint32_t minor_version = 0;
  cvd_cal_param_v1_t cal_param;

  #define LUT_ENTRY_CDFT_OFFSET ( table_object->num_columns *\
                                  cvd_cal_get_type_width( table_object->columns->type )\
                                )

  #define LUT_ENTRY_CDOT_OFFSET ( ( table_object->num_columns *\
                                  cvd_cal_get_type_width( table_object->columns->type )\
                                  ) + ( sizeof( uint32_t ) )\
                                )
  for ( ;; )
  {
    rc = cvd_cal_get_minor_version( cal_query_handle, &minor_version );
    if ( rc )
    {
      rc = APR_EBADPARAM;
      break;
    }

    if ( cb_fn == NULL )
    {
      MSG ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
              "call back function pointer cannot be null" );
      rc = APR_EBADPARAM;
      break;
    }

    rc = cvd_cal_get_typed_object( cal_query_handle, CVD_CAL_OBJECT_TYPE_ENUM_QUERY,
                                   ( cvd_cal_object_t** ) &query_object );
    if ( rc )
    {
      MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
              "invalid query handle = %d", cal_query_handle );
      break;
    }

    rc = cvd_cal_get_typed_object( query_object->table_handle,
                                   CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
                                   ( cvd_cal_object_t** ) &table_object );
    if ( rc )
    {
      MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
              "invalid Table handle = %d", query_object->header.handle );
      break;
    }
    matching_entries_array = ( uint32_t* ) query_object->matching_entries_buffer;

    /* Get the first LUT entry start.*/
    lut_entry_start = ( uint8_t* ) table_object->lut_start + sizeof( uint32_t );
    lut_entry_size =  ( ( table_object->num_columns *
                          cvd_cal_get_type_width( table_object->columns->type )
                        ) + ( sizeof( uint32_t ) * 2 )
                      );
    MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_query():"
            "Query Handle = 0x%08x Number of Matching entries = %d",
            cal_query_handle, query_object->num_matching_entries );

    /* Extract cal data for one matching entry. */
    while ( i < query_object->num_matching_entries )
    {
      lut_offset = ( matching_entries_array[ i ] * lut_entry_size );
      lut_entry = lut_entry_start + lut_offset;

      cdft_offset = *( ( uint32_t* ) ( lut_entry + LUT_ENTRY_CDFT_OFFSET ) );
      cdft_index_ptr = ( uint32_t* )( ( uint8_t* )table_object->cdft_start + cdft_offset );
      num_mid_pid_pairs = *cdft_index_ptr;

      cdot_offset = *( ( uint32_t* ) ( lut_entry + LUT_ENTRY_CDOT_OFFSET ) );
      cdot_index_ptr = ( uint32_t* )( ( uint8_t* )table_object->cdot_start + cdot_offset );
      num_data_offset_entries = *cdot_index_ptr;

      /* Each ModuleID - ParamID pair whould have a corresponding cal data. */
      if ( num_data_offset_entries != num_mid_pid_pairs )
      {
        MSG_3 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
                "CDFT and CDOT entries mismatch. num_mid_pid_pairs = %d,"
                "num_data_offset_entries =%d, at entry index %d",
                num_mid_pid_pairs, num_data_offset_entries, i );
      }
      cdot_index_ptr++;
      cdft_index_ptr++;
      
      /* Repeat for all ModuleID - ParamID pairs. */
      while ( num_mid_pid_pairs && num_data_offset_entries )
      {
        cal_param.minor_version = CVD_CAL_PARAM_MINOR_VERSION_1; 
        cal_param.module_id = *cdft_index_ptr;
        cdft_index_ptr++;
        if ( CVD_CAL_PARAM_MINOR_VERSION_1 == table_object->minor_version )
        {
          cal_param.instance_id = *(uint16_t*)cdft_index_ptr;  
          cal_param.reserved = 0;
          cdft_index_ptr++;
        }
        else
        {
          cal_param.instance_id = 0;  
          cal_param.reserved = 0;
        }
        cal_param.param_id = *cdft_index_ptr; 
        data_offset = *cdot_index_ptr;
        data_index_ptr = ( uint32_t* ) ( (uint8_t*)table_object->data_start + data_offset );

        cal_param.param_data_size = *data_index_ptr;
        data_index_ptr++;
        cal_param.param_data = ( void* ) data_index_ptr;

        cdft_index_ptr++;
        cdot_index_ptr++;
        num_mid_pid_pairs--;
        num_data_offset_entries--;

        /* Verify alignment */
        rc = cvd_mem_mapper_validate_access_align( cal_param.param_data,
               cal_param.param_data_size, CVD_CAL_DATA_ALIGNMENT );
        if ( APR_EOK != rc)
        {
          MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
                  "Invalid alignment at address 0x%x", cal_param.param_data );
          return APR_EFAILED;
        }else if ( cal_param.param_data_size >= table_object->data_size )
        {
          MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_query():"
                  "out of bound param_data_size %d at address 0x%x",
                   cal_param.param_data_size, cal_param.param_data );
          return APR_EFAILED;
        }

        /* send cal data to the client through call back function. */

        /* WV enable/disable is controlled via device config table. However,
           WV MID and mod_enable PID can be present in calib data as well.
           Below check is for skipping the set param if below MID/PID pair
           is present in cal data. */

        if ( ( ( VOICE_MODULE_WV != cal_param.module_id ) &&
               ( VOICE_MODULE_WV_V2 != cal_param.module_id ) ) ||
               ( VOICE_PARAM_MOD_ENABLE != cal_param.param_id ) )
        {
           cb_fn( ( void* )&cal_param, ( void* )client_data );
        }
        cal_param_count++;
      }
      i++;
    }
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_query():"
            "cal_param_count = %d", cal_param_count );
    break;
  }
  #undef LUT_ENTRY_CDFT_OFFSET
  #undef LUT_ENTRY_CDOT_OFFSET

  return rc;
}

/* Initialize the CVD calibration utility. */
APR_INTERNAL int32_t cvd_cal_init (
  void
)
{
  apr_objmgr_setup_params_t params;

  ( void ) apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &cvd_cal_int_lock );

  apr_memmgr_init_heap( &cvd_cal_heapmgr, ( ( void* ) &cvd_cal_heap_pool ),
                        sizeof( cvd_cal_heap_pool ), NULL, NULL );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_cal_init(): done ======" );

  params.table = cvd_cal_object_table;
  params.total_bits = CVD_CAL_HANDLE_TOTAL_BITS_V;
  params.index_bits = CVD_CAL_HANDLE_INDEX_BITS_V;
  params.lock_fn = cvd_cal_int_lock_fn;
  params.unlock_fn = cvd_cal_int_unlock_fn;
  ( void ) apr_objmgr_construct( &cvd_cal_objmgr, &params );

  ( void ) cvd_cal_log_init( );

  return APR_EOK;
}


/* Deinitialize the CVD calibration utility. */
APR_INTERNAL int32_t cvd_cal_deinit (
  void
)
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_cal_deinit(): done ======" );

  (void)cvd_cal_log_deinit();

  { /* Release the object management. */
    ( void ) apr_objmgr_destruct( &cvd_cal_objmgr );
  }
  { /* Release the locks. */
    ( void ) apr_lock_destroy( cvd_cal_int_lock );
  }

  return APR_EOK;
}

int32_t cvd_cal_utility_remove_na_matching_entries(
  uint32_t query_handle,
  uint32_t column_id
)
{
  int32_t rc = APR_EOK;
  cvd_cal_query_object_t* query_object = NULL;
  cvd_cal_table_object_t* table_object = NULL;
  uint8_t* lut_entry = NULL;
  uint32_t lut_entry_size;
  uint32_t lut_offset;
  uint8_t* lut_entry_start = NULL;
  uint32_t* matching_entries_array = NULL;
  uint32_t column_offset;
  uint32_t column_type;
  uint32_t column_na_value;
  uint32_t* lut_entry_column = NULL;
  int i = 0, j = 0;

  rc = cvd_cal_get_typed_object( query_handle, CVD_CAL_OBJECT_TYPE_ENUM_QUERY,
                                 ( cvd_cal_object_t** )&query_object );
  if ( rc )
  {
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_utility_remove_na_matching_entries():"
            "invalid query handle = 0x%08X", query_handle );
    return rc;
  }

  rc = cvd_cal_get_typed_object( query_object->table_handle,
                                 CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
                                 ( cvd_cal_object_t** )&table_object );
  if ( rc )
  {
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_utility_remove_na_matching_entries():"
            "invalid Table handle = 0x%08X", query_object->header.handle );
    return rc;
  }

  matching_entries_array = ( uint32_t* )query_object->matching_entries_buffer;

  /* Get the first LUT entry start.*/
  lut_entry_start = ( uint8_t* ) table_object->lut_start + sizeof( uint32_t );
  lut_entry_size =  ( ( table_object->num_columns *
                        cvd_cal_get_type_width( table_object->columns->type )
                      ) + ( sizeof( uint32_t ) * 2 )
                    );
  MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_utility_remove_na_matching_entries():"
          "Query Handle = 0x%08X Number of Matching entries before removal = %d",
          query_handle, query_object->num_matching_entries );

  /* Find volume index offset and corresponding NA value. */
  rc = cvd_cal_get_column_offset_and_info_v2(
         table_object, column_id,
         &column_offset, &column_type, &column_na_value );
  if ( rc )
  {
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_utility_remove_na_matching_entries():"
            "Column ID not found Table handle = %d", table_object->header.handle );
    return rc;
  }

  /* search the matched entries for a match with NA value 
   * if match found, delete the entry from matched list.
   */

  while ( i < query_object->num_matching_entries )
  {
    lut_offset = ( matching_entries_array[ i ] * lut_entry_size );
    lut_entry = lut_entry_start + lut_offset;

    lut_entry_column = ( uint32_t* )( lut_entry + column_offset );

    if ( *lut_entry_column == column_na_value )
    {
      j = i;
      while ( ( j + 1 ) < query_object->num_matching_entries )
      {
        matching_entries_array[ j ] = matching_entries_array[ j + 1 ];
        j++;
      }
      query_object->num_matching_entries -= 1;
      continue;
    }

    i++;
  }

  MSG_2 ( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_utility_remove_na_matching_entries():"
          "Query Handle = 0x%08X Number of Matching entries after removal = %d",
          query_handle, query_object->num_matching_entries );

  return rc;
}

APR_INTERNAL uint32_t cvd_cal_obj_stat (
  uint32_t *objects_used
)
{
  if( NULL == objects_used )
  {
    return APR_EBADPARAM;
  }
  *objects_used = CVD_CAL_MAX_OBJECTS_V - cvd_cal_objmgr.free_q.size;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "cvd_cal_obj_stat(): Objects used=(%d)",
                                        *objects_used );
  return APR_EOK;
}

