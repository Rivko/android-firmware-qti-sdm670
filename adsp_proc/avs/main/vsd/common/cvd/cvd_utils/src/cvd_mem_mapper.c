/*
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_mem_mapper.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmstd.h"
#include "msg.h"
#include "err.h"

#ifdef WINSIM
  #include <stdlib.h>
#elif defined( USE_QURT_MEM_MAPPER )
  #include "qurt.h"
#else
  #error "This platform is unsupported."
#endif /* WINSIM */

#include "apr_errcodes.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"
#include "apr_lock.h"

#include "vss_private_if.h"
#include "cvd_mem_mapper_i.h"

#ifndef WINSIM
#include "smem.h"
#endif
/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_MEM_MAPPER_HEAP_SIZE_V ( 2048 )

#define CVD_MEM_MAPPER_HANDLE_TOTAL_BITS_V ( 16 )

#define CVD_MEM_MAPPER_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */

#define CVD_MEM_MAPPER_MAX_OBJECTS_V ( 1 << CVD_MEM_MAPPER_HANDLE_INDEX_BITS_V )

#define CVD_MEM_MAPPER_ACCESS_MASK_READ ( 1 )

#define CVD_MEM_MAPPER_ACCESS_MASK_WRITE ( 2 )

#define CVD_MEM_MAPPER_SUPPORTED_IS_CACHED ( TRUE )

#define CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN ( 4096 )

#define CVD_MEM_MAPPER_SUPPORTED_MIN_DATA_WIDTH ( 8 )

#define CVD_MEM_MAPPER_SUPPORTED_MAX_DATA_WIDTH ( 64 )

#define CVD_MEM_MAPPER_INVALID_ADDR ( 0xFFFFFFFF )

#define CVD_MEM_MAPPER_INVALID_ADDR_64 ( 0xFFFFFFFFFFFFFFFF )

#define CVD_MEM_MAPPER_MAX_TABLES ( 10 )

#define CVD_MEM_MAPPER_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

/* Size of the reserved SMEM_VOICE region. */
#define CVD_MEM_MAPPER_SMEM_HEAP_SIZE ( 81920 )

/* The first 8 KB of the SMEM_VOICE region are reserved for use by ECall.
 * Therefore the SMEM heap available for VOICEMEM purposes starts at 8 KB and
 * only 72 KB are available. */
#define CVD_MEM_MAPPER_SMEM_HEAP_OFFSET ( 8192 )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef enum cvd_mem_mapper_mem_type_enum_t
{
  CVD_MEM_MAPPER_MEM_TYPE_ENUM_UNINITIALIZED,
  CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL,
  CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS,
  CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS,
  CVD_MEM_MAPPER_MEM_TYPE_ENUM_INVALID,
}
  cvd_mem_mapper_mem_type_enum_t;

typedef struct cvd_mem_mapper_region_object_t
{
  uint32_t handle;
  cvd_mem_mapper_mem_type_enum_t type;
  uint32_t num_mem_blocks_mapped;
  uint64_t base_virt_addr;
  uint64_t base_phys_addr;
    /**< Valid only for physical contiguous memory. */
  uint32_t size;
  bool_t is_cached;
  bool_t is_unmap_needed;
  uint16_t cache_line_size;
  uint32_t access_mask;
  uint32_t page_align;
  uint8_t min_data_width;
  uint8_t max_data_width;
  uint32_t access_align;
  uint32_t vsm_mem_handle;
  uint32_t vpm_mem_handle;
#if defined( USE_QURT_MEM_MAPPER )
  qurt_addr_t mem_table_virt_addresses [ CVD_MEM_MAPPER_MAX_TABLES ];
  qurt_size_t mem_table_align_size [ CVD_MEM_MAPPER_MAX_TABLES ];
#endif /* USE_QURT_MEM_MAPPER */
}
  cvd_mem_mapper_region_object_t;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Lock Management */
static apr_lock_t cvd_mem_mapper_objmgr_lock;

/* Heap Management */
static uint8_t cvd_mem_mapper_heap_pool[ CVD_MEM_MAPPER_HEAP_SIZE_V ];
static apr_memmgr_type cvd_mem_mapper_heapmgr;

/* Object Management */
static apr_objmgr_object_t cvd_mem_mapper_object_table[ CVD_MEM_MAPPER_MAX_OBJECTS_V ];
static apr_objmgr_t cvd_mem_mapper_objmgr;

/* Flag that indicates whether CVD mem mapper inialized succesfully. */
static bool_t cvd_mem_mapper_is_initialized = TRUE;

/* New Voice Memory Pool */
static const char cvd_mem_mapper_pool_name[ 32 ] = "EBI1_pool";

#ifndef WINSIM
/* SMEM heap manager. */
static void* cvd_mem_mapper_smem_heap_virt_addr;
uint32_t smem_heap_size;
#endif

#if defined( USE_QURT_MEM_MAPPER )
static qurt_mem_pool_t cvd_mem_mapper_pool;
#endif

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

/****************************************************************************
 * ROUTINES                                                                 *
 ****************************************************************************/

static void cvd_mem_mapper_objmgr_lock_fn ( void )
{
  ( void ) apr_lock_enter( cvd_mem_mapper_objmgr_lock );
}

static void cvd_mem_mapper_objmgr_unlock_fn ( void )
{
  ( void ) apr_lock_leave( cvd_mem_mapper_objmgr_lock );
}

/* Round up input to multiple of align. */
static int32_t cvd_mem_mapper_round_up_align (
  uint32_t input,
  uint32_t align,
  uint32_t* ret_aligned_output
)
{
  if ( ret_aligned_output == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_aligned_output = ( ( input + align - 1 ) & ( ~ ( align - 1 ) ) );

  return APR_EOK;
}

static int32_t cvd_mem_mapper_create_region_object (
  cvd_mem_mapper_region_object_t** ret_region_obj
)
{
  int32_t rc;
  uint32_t checkpoint = 0;
  cvd_mem_mapper_region_object_t* region_obj;
  apr_objmgr_object_t* objmgr_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    { /* Allocate memory for the new region object. */
      region_obj = apr_memmgr_malloc( &cvd_mem_mapper_heapmgr, 
                                      sizeof( cvd_mem_mapper_region_object_t ) );
      if ( region_obj == NULL )
      {
        rc = APR_ENORESOURCE;
        break;
      }
      checkpoint = 1;
    }

    { /* Allocate a new handle for the region object. */
      rc = apr_objmgr_alloc_object( &cvd_mem_mapper_objmgr, &objmgr_obj );
      if ( rc ) break;
      checkpoint = 2;
    }

    /* Link the region object to the handle */
    objmgr_obj->any.ptr = region_obj;

    { /* Initialize the region object. */
      mmstd_memset( region_obj, 0, sizeof( cvd_mem_mapper_region_object_t ) );
      region_obj->handle = objmgr_obj->handle;
    }

    *ret_region_obj = region_obj;

    return APR_EOK;
  }
  
  switch ( checkpoint )
  {
  case 2:
    ( void ) apr_objmgr_free_object( &cvd_mem_mapper_objmgr, objmgr_obj->handle );
    /*-fallthru */

  case 1:
    apr_memmgr_free( &cvd_mem_mapper_heapmgr, region_obj );
    /*-fallthru */

  default:
    break;
  }

  return rc;
}

static int32_t cvd_mem_mapper_free_region_object (
  cvd_mem_mapper_region_object_t* region_obj
)
{
  if ( region_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &cvd_mem_mapper_objmgr, region_obj->handle );
  apr_memmgr_free( &cvd_mem_mapper_heapmgr, region_obj );

  return APR_EOK;
}

static int32_t cvd_mem_mapper_get_region_object (
  uint32_t handle,
  cvd_mem_mapper_region_object_t** ret_region_obj
)
{
  int32_t rc;
  apr_objmgr_object_t* objmgr_obj;
  cvd_mem_mapper_region_object_t* region_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    rc = apr_objmgr_find_object( &cvd_mem_mapper_objmgr, handle, &objmgr_obj );
    if ( rc ) break;

    region_obj = ( ( cvd_mem_mapper_region_object_t* ) objmgr_obj->any.ptr );
    if ( region_obj == NULL )
    {
      rc = APR_EFAILED;
      break;
    }

    *ret_region_obj = region_obj;

    return APR_EOK;
  }

  return rc;
}


/* Unmaps the memory tables. Assumes all the memory tables are mapped. */
static int32_t cvd_mem_mapper_unmap_mem_tables (
  uint32_t num_mem_tables_mapped,
  uint64_t first_table_phy_addr,
  cvd_mem_mapper_region_object_t* region_obj
)
{

#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;
  qurt_addr_t mem_table_virt_addr;
  qurt_mem_region_t mem_table_region;
  qurt_mem_region_attr_t mem_table_region_attr;

  while ( num_mem_tables_mapped != 0 )
  {
    qurt_mem_rc = qurt_mem_region_query_64( &mem_table_region, 
                                         region_obj->mem_table_virt_addresses[ num_mem_tables_mapped-1 ],
                                         ( ( qurt_paddr_64_t ) CVD_MEM_MAPPER_INVALID_ADDR_64 ) );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    qurt_mem_rc = qurt_mem_region_attr_get( mem_table_region, 
                                            &mem_table_region_attr );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    qurt_mem_region_attr_get_virtaddr( &mem_table_region_attr, &mem_table_virt_addr );

    qurt_mem_rc = qurt_mem_region_delete( mem_table_region );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    num_mem_tables_mapped--;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

/* Maps all the memory tables and retrieves the total memory size of all the 
 * memory blocks and the number of tables mapped.
 */
static int32_t cvd_mem_mapper_map_mem_tables (
  vss_imemory_table_descriptor_t* first_table_descriptor,
  uint32_t* ret_total_mem_size,
  uint32_t* ret_num_tables_mapped,
  cvd_mem_mapper_region_object_t* region_obj
)
{
  int32_t rc = APR_EOK;

#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;
  vss_imemory_table_descriptor_t* mem_table_descriptor;
  qurt_paddr_64_t mem_table_phys_addr;
  qurt_addr_t mem_table_virt_addr;
  qurt_size_t mem_table_size;
  qurt_size_t mem_table_aligned_size;
  uint32_t num_mem_tables_mapped = 0;
  qurt_mem_region_t mem_table_region;
  qurt_mem_region_attr_t mem_table_region_attr;
  uint32_t num_mem_blocks;
  uint32_t block_num;
  vss_imemory_block_t* mem_block;
  uint32_t total_mem_size = 0;

  mem_table_phys_addr = ( ( qurt_paddr_64_t ) first_table_descriptor->mem_address );
  mem_table_size = ( ( qurt_size_t ) first_table_descriptor->mem_size );

  if ( ret_total_mem_size == NULL || ret_num_tables_mapped == NULL )
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

  if ( mem_table_size == 0 )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_map_mem_tables(): " \
          "Table size is 0" );
    return APR_EBADPARAM;
  }

  for ( ;; )
  {
    { 
      /* Map the memory table. */
      if ( mem_table_phys_addr & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_mem_tables(): Misaligned table address: " \
                "table# %d, table_addr=0x%08X",
                ( num_mem_tables_mapped + 1 ), mem_table_phys_addr );
        rc = APR_EBADPARAM;
        break;
      }

      if ( ( mem_table_size - sizeof( vss_imemory_table_descriptor_t ) ) % 
           sizeof( vss_imemory_block_t ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_mem_tables(): Invalid table size: table# " \
                "%d table_size=%d",
                ( num_mem_tables_mapped + 1 ), mem_table_size);
        rc = APR_EBADPARAM;
        break;
      }

      qurt_mem_rc = qurt_mem_pool_attach( 
                      ( ( char* ) cvd_mem_mapper_pool_name ), &cvd_mem_mapper_pool );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_ERROR( "cvd_mem_mapper_map_mem_tables(): Failed qurt_mem_pool_attach()",0,0,0);
        rc = APR_EFAILED;
        break;
      }

      ( void ) cvd_mem_mapper_round_up_align( 
                 ( ( uint32_t ) mem_table_size ), 
                 CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN,
                 ( ( uint32_t* ) &mem_table_aligned_size ) );

      qurt_mem_region_attr_init( &mem_table_region_attr );
      qurt_mem_region_attr_set_physaddr_64( &mem_table_region_attr,
                                       mem_table_phys_addr );
      qurt_mem_region_attr_set_mapping( &mem_table_region_attr, 
                                      QURT_MEM_MAPPING_PHYS_CONTIGUOUS );
      qurt_mem_rc = qurt_mem_region_create( &mem_table_region, 
                                          mem_table_aligned_size,
                                          cvd_mem_mapper_pool, &mem_table_region_attr );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_mem_tables(): Failed " \
                "qurt_mem_region_create(): table# %d result=0x%08X table_size = %d",
                (num_mem_tables_mapped+1),
                qurt_mem_rc,
                mem_table_aligned_size );
            rc = APR_EFAILED;
            break;
      }
      num_mem_tables_mapped++;
    }

    { /* Parse the table and accumulate the total memory size. */
      qurt_mem_rc = qurt_mem_region_attr_get( mem_table_region, &mem_table_region_attr );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
               "cvd_mem_mapper_map_mem_tables(): Failed " \
               "qurt_mem_region_attr_get(): mem_table_region 0x%0X result=0x%08X",
               mem_table_region, qurt_mem_rc );
        rc = APR_EFAILED;
        break;
      }

      qurt_mem_region_attr_get_virtaddr( &mem_table_region_attr,
                                         &mem_table_virt_addr );
      region_obj->mem_table_virt_addresses[ num_mem_tables_mapped-1 ] = mem_table_virt_addr;
      region_obj->mem_table_align_size[ num_mem_tables_mapped-1 ] = mem_table_aligned_size;

      ( void ) cvd_mem_mapper_cache_invalidate( mem_table_virt_addr,
                                                mem_table_aligned_size );

      mem_table_descriptor = ( ( vss_imemory_table_descriptor_t* ) 
                               mem_table_virt_addr );
      mem_block = ( ( vss_imemory_block_t* )
                    ( mem_table_virt_addr + 
                      sizeof( vss_imemory_table_descriptor_t ) ) );

      num_mem_blocks = ( ( mem_table_size - sizeof( vss_imemory_table_descriptor_t ) ) / 
                          sizeof( vss_imemory_block_t ) );

      block_num = 1;

      while ( num_mem_blocks != 0 )
      {
        if ( mem_block->mem_address & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_map_mem_tables(): misaligned block address: " \
                  "table# %d block# %d block_addr=0x%016X",
                  num_mem_tables_mapped, block_num, mem_block->mem_address );
          rc = APR_EBADPARAM;
          break;
        }

        if ( mem_block->mem_size & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_map_mem_tables(): misaligned block size: " \
                  "table# %d block# %d block_size=%d",
                  num_mem_tables_mapped, block_num, mem_block->mem_size );
          rc = APR_EBADPARAM;
          break;
        }

        total_mem_size += mem_block->mem_size;
        mem_block++;
        num_mem_blocks--;
        block_num++;
      }

      if ( rc ) break;

      { /* Update the address and size to point to next table if there is any. */
        mem_table_size = ( ( qurt_size_t ) mem_table_descriptor->mem_size );
        if ( mem_table_size == 0 )
        { /* No more tables to parse. */
          break;
        }

        mem_table_phys_addr = ( ( qurt_paddr_64_t ) mem_table_descriptor->mem_address );
      }
    }
  }

  if ( rc )
  { /* Unmap all the mapped memory tables. */
    ( void ) cvd_mem_mapper_unmap_mem_tables( 
               num_mem_tables_mapped,
               first_table_descriptor->mem_address,
               region_obj );
  }
  else
  {
    *ret_total_mem_size = total_mem_size;
    *ret_num_tables_mapped = num_mem_tables_mapped;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return rc;
}

/* Unmap all the memory blocks that were mapped. */
static int32_t cvd_mem_mapper_unmap_all_mem_blocks( 
  uint32_t num_mem_blocks,
  uint64_t base_virt_addr,
  cvd_mem_mapper_region_object_t* region_obj
)
{

#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;
  qurt_addr_t sub_region_virt_addr;
  qurt_paddr_64_t sub_region_phys_addr;
  qurt_size_t sub_region_size;
  qurt_mem_region_t sub_region;
  qurt_mem_region_attr_t sub_region_attr;
  qurt_mem_region_t master_region;
  
  sub_region_virt_addr = ( ( qurt_addr_t ) base_virt_addr );

  /* Unmap all the memory blocks. */
  while ( num_mem_blocks != 0 )
  {
    sub_region_phys_addr = qurt_lookup_physaddr_64( sub_region_virt_addr );
    if ( sub_region_phys_addr == 0 ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    {
      qurt_mem_rc = qurt_mem_region_query_64( 
                      &sub_region, ( ( qurt_addr_t ) CVD_MEM_MAPPER_INVALID_ADDR ),
                      sub_region_phys_addr );
      if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED ); 
    }

    qurt_mem_rc = qurt_mem_region_attr_get( sub_region, &sub_region_attr );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    qurt_mem_region_attr_get_size( &sub_region_attr, &sub_region_size );

    {
      qurt_mem_rc = qurt_mem_region_delete( sub_region );
      if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
    }
    sub_region_virt_addr += sub_region_size;
    num_mem_blocks--;
  }

  { /* Unmap the master region. */
    qurt_mem_rc = qurt_mem_region_query_64( 
                    &master_region, base_virt_addr,
                    ( ( qurt_paddr_64_t ) CVD_MEM_MAPPER_INVALID_ADDR_64 ) );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

    qurt_mem_rc = qurt_mem_region_delete( master_region );
    if ( qurt_mem_rc != QURT_EOK ) CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

/* Maps all the memory blocks in all the memory tables. Assumes all the memory
 * tables are mapped and all the memory blocks have proper alignment and size.
 */
static int32_t cvd_mem_mapper_map_all_mem_blocks (
  vss_imemory_table_descriptor_t* first_mem_table_descriptor,
  uint32_t total_mem_size,
  uint64_t* ret_base_virtual_addr,
  uint64_t* ret_base_phys_addr, 
    /**< 
     * Valid only if there is a single memory block to map, 
     * i.e. physical contiguous mapping. 
     */
  uint32_t* ret_num_mem_blocks_mapped,
  cvd_mem_mapper_region_object_t* region_obj
)
{
  int32_t rc = APR_EOK;

#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;
  uint32_t checkpoint = 0;
  qurt_mem_region_t master_region;
  qurt_mem_region_attr_t master_region_attr;
  qurt_addr_t master_region_virt_addr=0;
  vss_imemory_table_descriptor_t* mem_table_descriptor;
  qurt_addr_t mem_table_virt_addr;
  uint32_t mem_table_size;
  qurt_mem_region_t mem_table_region;
  qurt_mem_region_attr_t mem_table_region_attr;
  vss_imemory_block_t* mem_block;
  uint32_t table_num = 1;
  uint32_t num_mem_blocks;
  uint32_t num_mem_blocks_mapped = 0;
  qurt_mem_region_t sub_region;
  qurt_mem_region_attr_t sub_region_attr;
  qurt_addr_t sub_region_virt_addr;

  for ( ;; )
  {
    { /* Reserve a range of virtual memory. */
      qurt_mem_region_attr_init( &master_region_attr );
      qurt_mem_region_attr_set_mapping( &master_region_attr, QURT_MEM_MAPPING_NONE );

      /* The mem pool is ignored by QuRT for mapping mode QURT_MEM_MAPPING_NONE.
       * QuRT will introduce a invalid pool ID definition to indicate that the
       * memory pool is ignored. Before this, it is recommended to use
       * qurt_mem_default_pool.
       */
      qurt_mem_rc = qurt_mem_region_create( &master_region, 
                                            ( ( qurt_size_t ) total_mem_size ),
                                            qurt_mem_default_pool, 
                                            &master_region_attr );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_all_mem_blocks(): Failed " \
                "qurt_mem_region_create(): result=0x%08X",
                qurt_mem_rc );
        rc = APR_EFAILED;
        break;
      }
      checkpoint = 1;

      qurt_mem_rc = qurt_mem_region_attr_get( master_region, &master_region_attr );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
               "cvd_mem_mapper_map_all_mem_blocks(): Failed " \
               "qurt_mem_region_attr_get(): master_region 0x%0X result=0x%08X",
               master_region, qurt_mem_rc );
        rc = APR_EFAILED;
        break;
      }

      qurt_mem_region_attr_get_virtaddr( &master_region_attr, 
                                         &master_region_virt_addr );
    }

    mem_table_descriptor = first_mem_table_descriptor;
    sub_region_virt_addr = master_region_virt_addr;

    for ( ;; )
    {
      { /* Determine the number of blocks in the table. Obtain the pointer to
         * the first memory block in the table. Update the mem table descriptor
         * to point to the next table. 
         */
        mem_table_size = mem_table_descriptor->mem_size;
        if ( mem_table_size == 0 )
        { /* No more tables to parse. */
          break;
        }

        qurt_mem_rc = qurt_mem_region_query_64( 
                        &mem_table_region, 
                        region_obj->mem_table_virt_addresses[ table_num-1 ],
                        ( ( qurt_paddr_64_t ) CVD_MEM_MAPPER_INVALID_ADDR_64 ) );
        if ( qurt_mem_rc != QURT_EOK )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_map_all_mem_blocks(): Falied " \
                  "qurt_mem_reqion_query(): table# %d result=0x%08X",
                  table_num, qurt_mem_rc );
          rc = APR_EFAILED;
          break;
        }

        qurt_mem_rc = qurt_mem_region_attr_get( mem_table_region, 
                                                &mem_table_region_attr );
        if ( qurt_mem_rc != QURT_EOK )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_map_all_mem_blocks(): Failed " \
                  "qurt_mem_region_attr_get(): table# %d result=0x%08X",
                  table_num, qurt_mem_rc );
          rc = APR_EFAILED;
          break;
        }

        qurt_mem_region_attr_get_virtaddr( &mem_table_region_attr, 
                                           &mem_table_virt_addr );

        mem_block = ( ( vss_imemory_block_t* )
                      ( mem_table_virt_addr + 
                        sizeof( vss_imemory_table_descriptor_t ) ) );
        mem_table_size -= sizeof( vss_imemory_table_descriptor_t );
        num_mem_blocks = ( mem_table_size / sizeof( vss_imemory_block_t ) );

        mem_table_descriptor = ( ( vss_imemory_table_descriptor_t* ) mem_table_virt_addr );
      }

      qurt_mem_rc = qurt_mem_pool_attach( 
                      ( ( char* ) cvd_mem_mapper_pool_name ), &cvd_mem_mapper_pool );
      if ( qurt_mem_rc != QURT_EOK )
      {
        MSG_ERROR( "cvd_mem_mapper_map_all_mem_blocks(): Failed qurt_mem_pool_attach()",0,0,0);
        rc = APR_EFAILED;
        break;
      }

      /* Map all the memory blocks in a table. */
      while ( num_mem_blocks != 0 )
      {
        qurt_mem_region_attr_init( &sub_region_attr );
        qurt_mem_region_attr_set_physaddr_64( 
          &sub_region_attr, ( ( qurt_paddr_64_t ) ( mem_block->mem_address ) ) );
        qurt_mem_region_attr_set_virtaddr( 
          &sub_region_attr, sub_region_virt_addr );
        qurt_mem_region_attr_set_mapping( 
          &sub_region_attr, QURT_MEM_MAPPING_VIRTUAL_FIXED );
        qurt_mem_region_attr_set_cache_mode( 
          &sub_region_attr, QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE );

        qurt_mem_rc = qurt_mem_region_create(
                        &sub_region, ( ( qurt_size_t ) mem_block->mem_size ),
                        cvd_mem_mapper_pool, &sub_region_attr );
        if ( qurt_mem_rc != QURT_EOK )
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_map_all_mem_blocks(): Failed " \
                  "qurt_mem_region_create(): table# %d block# %d result=0x%08X",
                  table_num, ( num_mem_blocks_mapped + 1 ), qurt_mem_rc );
              rc = APR_EFAILED;
              break;
            }

        /* ret_base_phys_addr is only valid if there is a single memory block to map. */
        *ret_base_phys_addr = mem_block->mem_address;

        num_mem_blocks_mapped++;
        num_mem_blocks--;
        sub_region_virt_addr += mem_block->mem_size;
        mem_block++;
      }

      if ( rc ) break;

      table_num++;
    }

    *ret_base_virtual_addr = ( ( uint64_t ) master_region_virt_addr );
    *ret_num_mem_blocks_mapped = num_mem_blocks_mapped;

    break;
  }

  if ( rc && ( checkpoint == 1 ) )
  { 
    ( void ) cvd_mem_mapper_unmap_all_mem_blocks( 
               num_mem_blocks_mapped, 
               ( ( uint64_t ) master_region_virt_addr ),
               region_obj);
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return rc;
}

APR_INTERNAL int32_t cvd_mem_mapper_init ( void )
{

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== cvd_mem_mapper_init()======" );

  { /* Initialize the lock. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &cvd_mem_mapper_objmgr_lock );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &cvd_mem_mapper_heapmgr, 
                          ( ( void* ) &cvd_mem_mapper_heap_pool ),
                          sizeof( cvd_mem_mapper_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;

    params.table = cvd_mem_mapper_object_table;
    params.total_bits = CVD_MEM_MAPPER_HANDLE_TOTAL_BITS_V;
    params.index_bits = CVD_MEM_MAPPER_HANDLE_INDEX_BITS_V;
    params.lock_fn = cvd_mem_mapper_objmgr_lock_fn;
    params.unlock_fn = cvd_mem_mapper_objmgr_unlock_fn;
    ( void ) apr_objmgr_construct( &cvd_mem_mapper_objmgr, &params );
  }
	
  { /* Allocate SMEM voice heap and initialize the VOICEMEM SMEM heap manager. */
#ifndef WINSIM
   /* Allocate SMEM heap. */
    
   /* NOTE: The 80 KB SMEM_VOICE region is shared between voice and ECall. ECall
       * allocates the first 8 KB. SMEM does not do heap management and there is no
       * smem_free() (hence the need for this VOICEMEM driver) therefore when a client
       * allocates a certain size from one of the predefined SMEM regions, other
       * clients that subsequently allocate from the same SMEM region will get the
       * same address and size allocated by the first client. Therefore it is imperative
       * that VOICEMEM is initialized BEFORE ECall, so that we can get the full 80 KB.
  */
#ifndef SIM
    cvd_mem_mapper_smem_heap_virt_addr = smem_alloc( SMEM_VOICE, CVD_MEM_MAPPER_SMEM_HEAP_SIZE );

    if ( cvd_mem_mapper_smem_heap_virt_addr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_init(): Failed to allocate SMEM heap" );
      //return APR_EFAILED;
    }

    cvd_mem_mapper_smem_heap_virt_addr = ( void*) ( ( ( ( uint32_t )cvd_mem_mapper_smem_heap_virt_addr + 4096 - 1 )&
                                                      ( ~( 4096 - 1 ) ) ) + CVD_MEM_MAPPER_SMEM_HEAP_OFFSET );

  
    ( void ) smem_get_addr( SMEM_VOICE, &smem_heap_size );
  
    /* Check that the amount of heap SMEM allocated matches what we asked for. */
    if ( smem_heap_size < CVD_MEM_MAPPER_SMEM_HEAP_SIZE )
    {
      MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_mem_mapper_init(): Insufficient SMEM_VOICE heap allocated: requested %d, allocated %d",
                                             CVD_MEM_MAPPER_SMEM_HEAP_SIZE, smem_heap_size );
  
      /* smem_free( cvd_mem_mapper_smem_heap_virt_addr ); */ /* smem_free() is not actually implemented. */
      //return APR_EFAILED;
    }
    else
    {
      MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_mem_mapper_init(): Allocated SMEM_VOICE heap: address 0x%0X8, size %d",
                                             cvd_mem_mapper_smem_heap_virt_addr, smem_heap_size );
    }
#endif  /* SIM_DEFINED */

#endif /* WINSIM */
  }
  
  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== " \
       "cvd_mem_mapper_deinit()======" );

  { /* Release the object management. */
    ( void ) apr_objmgr_destruct( &cvd_mem_mapper_objmgr );
  }

  { /* Release the lock. */
    ( void ) apr_lock_destroy( cvd_mem_mapper_objmgr_lock );
  }

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_cache_invalidate (
  uint64_t virt_addr,
  uint32_t size
)
{
#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;

  qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) virt_addr ), size,
                                      QURT_MEM_CACHE_INVALIDATE, 
                                      QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_cache_invalidate(): Failed " \
            "qurt_mem_cache_clean(): addr=0x%016X size=%d result=0x%08X",
            virt_addr, size, qurt_mem_rc );
    return APR_EFAILED;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_cache_invalidate_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
)
{
#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;

  qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) virt_addr->ptr ), size,
                                      QURT_MEM_CACHE_INVALIDATE, 
                                      QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_cache_invalidate_v2(): Failed " \
            "qurt_mem_cache_clean(): addr=0x%016X size=%d result=0x%08X",
            virt_addr->ptr, size, qurt_mem_rc );
    return APR_EFAILED;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_cache_flush (
  uint64_t virt_addr,
  uint32_t size
)
{
#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;

  qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) virt_addr ), size,
                                      QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_cache_flush(): " \
            "Failed qurt_mem_cache_clean(): addr=0x%016X size=%d " \
            "result=0x%08X",
            virt_addr, size, qurt_mem_rc );
    return APR_EFAILED;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_cache_flush_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
)
{
#if defined( WINSIM )

  /* Not needed (yet) on the simulator. */

#elif defined( USE_QURT_MEM_MAPPER )

  int qurt_mem_rc;

  qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) virt_addr->ptr ), size,
                                      QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE );
  if ( qurt_mem_rc != QURT_EOK )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_cache_flush(): " \
            "Failed qurt_mem_cache_clean(): addr=0x%08X size=%d " \
            "result=0x%08X",
            virt_addr->ptr, size, qurt_mem_rc );
    return APR_EFAILED;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_map_virtual (
  vss_imemory_cmd_map_virtual_t* map_virtual_arg,
  uint32_t* ret_mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  if ( map_virtual_arg == NULL )
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_map_virtual(): " \
         "mem_addr_lsw = 0x%08X mem_addr_msw = 0x%08X mem_size = %d",
         ( uint32_t )( map_virtual_arg->block.mem_address ),
         ( uint32_t )( map_virtual_arg->block.mem_address >> 32 ),
         map_virtual_arg->block.mem_size );
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_map_virtual(): " \
         "is_cached = %d cache_line_size = %d access_mask = %d", map_virtual_arg->is_cached,
         map_virtual_arg->cache_line_size, map_virtual_arg->access_mask );
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_map_virtual(): " \
         "page_align = %d min_data_width = %d max_data_width = %d",
         map_virtual_arg->page_align, map_virtual_arg->min_data_width,
         map_virtual_arg->max_data_width );

  /* Validate input parameters. */
  if ( ( map_virtual_arg->is_cached != CVD_MEM_MAPPER_SUPPORTED_IS_CACHED ) ||
       ( map_virtual_arg->cache_line_size != CVD_MEM_MAPPER_SUPPORTED_CACHE_LINE_SIZE ) ||
       ( map_virtual_arg->page_align != CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN ) ||
       ( map_virtual_arg->min_data_width != CVD_MEM_MAPPER_SUPPORTED_MIN_DATA_WIDTH ) ||
       ( map_virtual_arg->max_data_width != CVD_MEM_MAPPER_SUPPORTED_MAX_DATA_WIDTH ) ||
       ( map_virtual_arg->block.mem_address & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) ) ||
       ( map_virtual_arg->block.mem_size & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) ) ||
       ( ( ( map_virtual_arg->access_mask & CVD_MEM_MAPPER_ACCESS_MASK_READ ) ||
           ( map_virtual_arg->access_mask & CVD_MEM_MAPPER_ACCESS_MASK_WRITE ) ) != TRUE ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_map_virtual(): " \
          "Invalid params!" );
    return APR_EBADPARAM;
  }

  /* For virtual memory, there is only a need to create a region object to keep
   * track of the memory region information. The memory has already been mapped,
   * i.e. a TLB entry is already available.
   */
  for ( ;; )
  {
    { /* Create and initialize region object. */
      rc = cvd_mem_mapper_create_region_object( &region_obj );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_virtual(): Failed to create region obj: " \
                "result=0x%08X",
                rc );
        break;
      }

      region_obj->type = CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL;
      region_obj->base_virt_addr = map_virtual_arg->block.mem_address;
      region_obj->size = map_virtual_arg->block.mem_size;
      region_obj->is_cached = map_virtual_arg->is_cached;
      region_obj->cache_line_size = map_virtual_arg->cache_line_size;
      region_obj->access_mask = map_virtual_arg->access_mask;
      region_obj->page_align = map_virtual_arg->page_align;
      region_obj->min_data_width = map_virtual_arg->min_data_width;
      region_obj->max_data_width = map_virtual_arg->max_data_width;
      /* Any shared memory address/offset passed to CVD must meet the
       * access_align alignment requirement. The access_align is least
       * common multiple of max_data_width and cache line size. Since
       * only cached memory is supported, and cache line size is a 
       * multiple of max_data_width, the access_align is cache
       * line size.
       */
      region_obj->access_align = map_virtual_arg->cache_line_size;
    }

    *ret_mem_handle = region_obj->handle;

    return APR_EOK;
  }

  return rc;
}

/* Checks if the memory region falls into SMEM area and
   gets the corresponding memory block address and size. 
*/
static int32_t cvd_mem_mapper_validate_mem_is_in_smem ( 
  vss_imemory_table_descriptor_t* table_descriptor,
  uint32_t* ret_total_mem_size,
  uint64_t* ret_base_virtual_addr,
  uint64_t* ret_base_phys_addr, 
  cvd_mem_mapper_region_object_t* region_obj
)
{
  int32_t rc = APR_EFAILED;
  uint32_t smem_voice_size = 0;
#ifndef WINSIM 

  qurt_addr_t base_smem_virt_addr;
  qurt_paddr_64_t base_smem_phys_addr;
  qurt_addr_t table_virt_addr;
  vss_imemory_block_t* mem_block = NULL;
  qurt_paddr_64_t table_phys_addr;

  for ( ;; )
  {
    if ( ( NULL == ret_base_virtual_addr ) ||
         ( NULL == ret_base_phys_addr ) ||
         ( NULL == ret_total_mem_size ) ||
         ( NULL == region_obj ) )
    {
      rc = APR_EBADPARAM;
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
          " invalid params! ");
      break;
    }

    if ( NULL == cvd_mem_mapper_smem_heap_virt_addr )
    {
      rc = APR_EFAILED;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
          " Invalid smem base address!! ");
      break;
    }
    if ( CVD_MEM_MAPPER_SMEM_HEAP_SIZE != smem_heap_size )
    {
      rc = APR_EFAILED;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
          " size( %d ) mismatch!!  ", smem_heap_size);
      break;
    }
    
    base_smem_virt_addr = ( qurt_addr_t ) cvd_mem_mapper_smem_heap_virt_addr;
    smem_voice_size = smem_heap_size;

    base_smem_phys_addr = qurt_lookup_physaddr_64( ( qurt_addr_t ) base_smem_virt_addr );
    table_phys_addr = ( qurt_paddr_64_t ) table_descriptor->mem_address;
    if ( table_phys_addr & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_smem(): Misaligned table address: " \
              "table_addr=0x%08X",
               table_phys_addr );
      rc = APR_EBADPARAM;
      break;
    }
    
    if ( ( table_descriptor->mem_size - sizeof( vss_imemory_table_descriptor_t ) ) % 
         sizeof( vss_imemory_block_t ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_smem(): Invalid table size: " \
              "table_size=%d",
               table_descriptor->mem_size );
      rc = APR_EBADPARAM;
      break;
    }
    /* Check if the complete memory table shared by client falls into smem_voice. */
    if ( ( table_phys_addr >= base_smem_phys_addr ) &&
         ( ( table_phys_addr + table_descriptor->mem_size )  <= ( base_smem_phys_addr + smem_voice_size ) ) )
    {
      /* Get the physical offset of table address from base_phys and use it to get the virt table address. */
      table_virt_addr = ( qurt_addr_t )( base_smem_virt_addr + ( table_phys_addr - base_smem_phys_addr ) );
      mem_block = ( ( vss_imemory_block_t* )
                      ( table_virt_addr + 
                        sizeof( vss_imemory_table_descriptor_t ) ) );

      if ( mem_block->mem_address & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_validate_mem_is_in_smem(): misaligned block address: " \
                  "block_addr=0x%016X",
                   mem_block->mem_address );
          rc = APR_EBADPARAM;
          break;
        }

        if ( mem_block->mem_size == 0 )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_validate_mem_is_in_smem(): block size is 0: " \
                  "block_size=%d",
                  mem_block->mem_size );
          rc = APR_EBADPARAM;
          break;
        }

        if ( mem_block->mem_size & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                  "cvd_mem_mapper_validate_mem_is_in_smem(): misaligned block size: " \
                  "block_size=%d",
                  mem_block->mem_size );
          rc = APR_EBADPARAM;
          break;
        }

      /* Check if the complete memory block shared by client falls into smem_voice.
       * SMEM shall have only one memory block since its physically contiguous.
       */
      if ( ( mem_block->mem_address >= base_smem_phys_addr ) &&
         ( ( mem_block->mem_address + mem_block->mem_size )  <= ( base_smem_phys_addr + smem_voice_size ) ) )
      {
        *ret_base_phys_addr = ( uint64_t ) mem_block->mem_address;
        *ret_total_mem_size = ( uint32_t ) mem_block->mem_size;
        *ret_base_virtual_addr = ( uint64_t )( base_smem_virt_addr + ( ( ( qurt_addr_t ) *ret_base_phys_addr ) - base_smem_phys_addr ) );
        
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
            "  table_virt_addr 0x%x table_phys_addr 0x%x  base_phys_addr 0x%x", table_virt_addr, table_phys_addr, base_smem_phys_addr);
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
            "  ret_base_phys_addr 0x%x  ret_total_mem_size 0x%x  base_region_virt_addr 0x%x", *ret_base_phys_addr, *ret_total_mem_size, *ret_base_virtual_addr);

        region_obj->is_unmap_needed = FALSE;
        rc = APR_EOK;
      }
      else
      {
        rc = APR_EFAILED;
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
          " shared memory block does not fall into smem!! " );
      }
    }
    else
    {
      rc = APR_EFAILED;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_validate_mem_is_in_smem(): " \
          " shared memory table does not fall into smem!! " );
    }
    break;
  }
#endif /* WINSIM */
  return rc;
}

APR_INTERNAL int32_t cvd_mem_mapper_map_physical (
  vss_imemory_cmd_map_physical_t* map_physical_arg,
  uint32_t* ret_mem_handle
)
{
  int32_t rc;

#if defined( WINSIM )

  rc = APR_EUNSUPPORTED;

#elif defined( USE_QURT_MEM_MAPPER )

  uint32_t checkpoint = 0;
  uint32_t total_mem_size = 0;
  uint32_t num_mem_tables_mapped = 0;
  uint32_t num_mem_blocks_mapped=0;
  uint64_t base_region_virt_addr=0;
  uint64_t base_region_phys_addr = 0;
  cvd_mem_mapper_region_object_t* region_obj;

  if ( map_physical_arg == NULL )
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

  if ( cvd_mem_mapper_is_initialized != TRUE )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_map_physical(): " \
          "CVD_MEM_MAPPER not"
          " initalized succesfully!! " );
    return APR_EFAILED;
  }    

  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_map_physical(): " \
        "addr_lsw: 0x%08X, addr_msw: 0x%08X, size = %d," \
        "is_cached: %d, cache_line_size: %d, access_mask: %d",
        ( uint32_t )( map_physical_arg->table_descriptor.mem_address ),
        ( uint32_t )( map_physical_arg->table_descriptor.mem_address >> 32 ),
        map_physical_arg->table_descriptor.mem_size, map_physical_arg->is_cached,
        map_physical_arg->cache_line_size, map_physical_arg->access_mask );

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,  "cvd_mem_mapper_map_physical(): " \
        "page_align = %d min_data_width = %d max_data_width = %d",
        map_physical_arg->page_align, map_physical_arg->min_data_width,
        map_physical_arg->max_data_width );

  /* Validate input parameters. */
  if ( ( map_physical_arg->is_cached != CVD_MEM_MAPPER_SUPPORTED_IS_CACHED ) ||
       ( map_physical_arg->cache_line_size != CVD_MEM_MAPPER_SUPPORTED_CACHE_LINE_SIZE ) ||
       ( map_physical_arg->page_align != CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN ) ||
       ( map_physical_arg->min_data_width != CVD_MEM_MAPPER_SUPPORTED_MIN_DATA_WIDTH ) ||
       ( map_physical_arg->max_data_width != CVD_MEM_MAPPER_SUPPORTED_MAX_DATA_WIDTH ) ||
       ( map_physical_arg->table_descriptor.mem_address & ( CVD_MEM_MAPPER_SUPPORTED_PAGE_ALIGN - 1 ) ) ||
       ( map_physical_arg->table_descriptor.mem_size == 0 ) ||
       ( ( ( map_physical_arg->access_mask & CVD_MEM_MAPPER_ACCESS_MASK_READ ) ||
           ( map_physical_arg->access_mask & CVD_MEM_MAPPER_ACCESS_MASK_WRITE ) ) != TRUE ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_map_physical(): " \
          "Invalid params!" );
    return APR_EBADPARAM;
  }

  for ( ;; )
  {

    {/* Create region object to keep track of the memory region information. */
      rc = cvd_mem_mapper_create_region_object( &region_obj );

      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_map_physical(): Failed to create region obj: " \
                "result=0x%08X",
                rc );
        break;
      }
    }
    checkpoint = 1;

    rc = cvd_mem_mapper_validate_mem_is_in_smem( &map_physical_arg->table_descriptor,
                                        &total_mem_size,
                                        &base_region_virt_addr,
                                        &base_region_phys_addr,
                                        region_obj );
    if ( rc != APR_EOK )
    {
    /* Map all the memory tables and retrieve the virtual address of the first
     * table and the total memory size of all the memory blocks.*/
    rc = cvd_mem_mapper_map_mem_tables( &map_physical_arg->table_descriptor,
                                        &total_mem_size, 
                                        &num_mem_tables_mapped,
                                        region_obj );
    if ( rc ) 
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_map_physical(): Failed to map tables: " \
              "result=0x%08X",
              rc );
      break;
    }
    checkpoint = 2;

    /* Map all the memory blocks in all the memory tables. */
    rc = cvd_mem_mapper_map_all_mem_blocks( &map_physical_arg->table_descriptor,
                                            total_mem_size,
                                            &base_region_virt_addr,
                                            &base_region_phys_addr,
                                            &num_mem_blocks_mapped,
                                            region_obj );
    
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_map_physical(): Failed to map blocks: " \
              "result=0x%08X",
              rc );
      break;
    }
      region_obj->is_unmap_needed = TRUE;
    }


    { 
      if ( num_mem_blocks_mapped > 1 )
      {
        region_obj->type = CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS;
      }
      else
      {
        region_obj->type = CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS;
        region_obj->base_phys_addr = base_region_phys_addr;
      }

      region_obj->num_mem_blocks_mapped = num_mem_blocks_mapped;
      region_obj->base_virt_addr = base_region_virt_addr;
      region_obj->size = total_mem_size;
      region_obj->is_cached = map_physical_arg->is_cached;
      region_obj->cache_line_size = map_physical_arg->cache_line_size;
      region_obj->access_mask = map_physical_arg->access_mask;
      region_obj->page_align = map_physical_arg->page_align;
      region_obj->min_data_width = map_physical_arg->min_data_width;
      region_obj->max_data_width = map_physical_arg->max_data_width;
      /* Any shared memory address/offset passed to CVD must meet the
       * access_align alignment requirement. The access_align is least
       * common multiple of max_data_width and cache line size. Since
       * only cached memory is supported, and cache line size is a 
       * multiple of max_data_width, the access_align is cache
       * line size.
       */
      region_obj->access_align = map_physical_arg->cache_line_size;
    }

    /* Unmap all the mapped memory tables. */
    ( void ) cvd_mem_mapper_unmap_mem_tables( 
               num_mem_tables_mapped,
               map_physical_arg->table_descriptor.mem_address,
               region_obj );

    *ret_mem_handle = region_obj->handle;

    return APR_EOK;
  }

  /* Clean up. */
  switch ( checkpoint )
  {
  case 2:
    {

      ( void ) cvd_mem_mapper_unmap_mem_tables( 
                 num_mem_tables_mapped,
                 map_physical_arg->table_descriptor.mem_address,
                 region_obj );
    }
    /*-fallthru */

  case 1:
    {
      cvd_mem_mapper_free_region_object( region_obj );
    }
    /*-fallthru */

  default:
    break;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return rc;
}

APR_INTERNAL int32_t cvd_mem_mapper_unmap (
  uint32_t mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  for ( ;; )
  {
  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc ) 
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_unmap(): Invalid " \
            "mem_handle=0x%08X",
            mem_handle );
      break;
    }

    if ( FALSE == region_obj->is_unmap_needed )
    {
       ( void ) cvd_mem_mapper_free_region_object( region_obj );
       break;
  }

  switch ( region_obj->type )
  {
  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL:
    {
      ( void ) cvd_mem_mapper_free_region_object( region_obj );
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS:
  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS:
    {
      ( void ) cvd_mem_mapper_unmap_all_mem_blocks( 
                 region_obj->num_mem_blocks_mapped, region_obj->base_virt_addr, region_obj );
      ( void ) cvd_mem_mapper_free_region_object( region_obj );
    }
    break;

  default:
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }
    break;
  }
  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_get_virtual_addr (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint64_t* ret_virt_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
  uint64_t mem_region_phy_addr;
  uint64_t addr_offset;

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_virt_addr(): " \
            "Invalid mem_handle 0x%08X.",
            mem_handle );
    return rc;
  }

  switch ( region_obj->type )
  {
  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL:
    {
      if ( ( mem_addr < region_obj->base_virt_addr ) ||
           ( mem_addr >= ( region_obj->base_virt_addr + region_obj->size ) ) )
      {
        rc = APR_EBADPARAM;
        break;
      }

      *ret_virt_addr = mem_addr;
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS:
    {
      mem_region_phy_addr = region_obj->base_phys_addr;

      if ( ( mem_addr < mem_region_phy_addr ) ||
           ( mem_addr >= ( mem_region_phy_addr + region_obj->size ) ) )
      {
        rc = APR_EBADPARAM;
        break;
      }

      addr_offset = ( mem_addr - mem_region_phy_addr );
      *ret_virt_addr = ( region_obj->base_virt_addr + addr_offset );
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS:
    {
      if ( mem_addr >= region_obj->size )
      {
        rc = APR_EBADPARAM;
      }
      else
      {
        *ret_virt_addr = ( region_obj->base_virt_addr + mem_addr );
      }
    }
    break;

  default:
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

  return rc;
}

APR_INTERNAL int32_t cvd_mem_mapper_get_virtual_addr_v2 (
  uint32_t mem_handle,
  uint64_t mem_addr,
  cvd_virt_addr_t* ret_virt_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
  uint64_t mem_region_phy_addr;
  uint64_t addr_offset, final_addr;
  
  if ( NULL == ret_virt_addr )
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );

  /* Clear all fields */
  mmstd_memset( ret_virt_addr, 0, sizeof( cvd_virt_addr_t ) );

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_virt_addr(): " \
            "Invalid mem_handle 0x%08X.",
            mem_handle );
    return rc;
  }

  switch ( region_obj->type )
  {
  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL:
    {
      if ( ( mem_addr < region_obj->base_virt_addr ) ||
           ( mem_addr >= ( region_obj->base_virt_addr + region_obj->size ) ) )
      {
        rc = APR_EBADPARAM;
        break;
      }

      cvd_mem_mapper_set_virt_addr( ret_virt_addr, mem_addr );
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS:
    {
      mem_region_phy_addr = region_obj->base_phys_addr;

      if ( ( mem_addr < mem_region_phy_addr ) ||
           ( mem_addr >= ( mem_region_phy_addr + region_obj->size ) ) )
      {
        rc = APR_EBADPARAM;
        break;
      }

      addr_offset = ( mem_addr - mem_region_phy_addr );
      final_addr = region_obj->base_virt_addr + addr_offset;
      cvd_mem_mapper_set_virt_addr( ret_virt_addr, final_addr );
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS:
    {
      if ( mem_addr >= region_obj->size )
      {
        rc = APR_EBADPARAM;
      }
      else
      {
        final_addr = ( region_obj->base_virt_addr + mem_addr );
        cvd_mem_mapper_set_virt_addr( ret_virt_addr, final_addr );
      }
    }
    break;

  default:
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

  return rc;
}

/* This function assumes ret_mem_addr is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_mem_addr (
  uint32_t mem_handle,
  uint64_t virt_addr,
  uint64_t* ret_mem_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
#if defined( USE_QURT_MEM_MAPPER )
  qurt_paddr_64_t phys_addr;
#endif

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_mem_addr(): " \
            "Invalid mem_handle=0x%08X",
            mem_handle );
    return rc;
  }

  if ( ( virt_addr < region_obj->base_virt_addr ) ||
       ( virt_addr >= ( region_obj->base_virt_addr + region_obj->size ) ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_mem_addr(): " \
            "Virt_addr 0x%016X is not in the region pointed to by the " \
            "mem_handle 0x%08X",
            virt_addr, mem_handle );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_mem_addr(): " \
            "Starting virt_addr of the region pointed to by the mem_handle is " \
            "0x%016X",
            region_obj->base_virt_addr );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "cvd_mem_mapper_get_mem_addr(): " \
            "Size of the region pointed to by the mem_handle is %d",
            region_obj->size );

    return APR_EBADPARAM;
  }

#if defined( WINSIM )

  *ret_mem_addr = virt_addr;

#elif defined( USE_QURT_MEM_MAPPER )

  switch ( region_obj->type )
  {
  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_VIRTUAL:
    {
      *ret_mem_addr = virt_addr;
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_CONTIGUOUS:
    {
      phys_addr = qurt_lookup_physaddr_64( ( ( qurt_addr_t ) virt_addr ) );
      if ( phys_addr == 0 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
                "cvd_mem_mapper_get_mem_addr(): Unable to get the phys_addr " \
                "for virt_addr=0x%016X",
                virt_addr );
        rc = APR_EBADPARAM;
        break;
      }
      
      *ret_mem_addr = ( ( uint64_t ) phys_addr );
    }
    break;

  case CVD_MEM_MAPPER_MEM_TYPE_ENUM_PHYSICAL_NON_CONTIGUOUS:
    {
      *ret_mem_addr = ( virt_addr - region_obj->base_virt_addr );
    }
    break;

  default:
    CVD_MEM_MAPPER_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

#else

  #error "This platform is unsupported."

#endif /* WINSIM */

  return rc;
}

/* This function assumes ret_base_virt_addr is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_region_base_virt_addr (
  uint32_t mem_handle,
  uint64_t* ret_base_virt_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_get_region_base_virt_addr(): Invalid " \
            "mem_handle=0x%08X",
            mem_handle );
    return rc;
  }

  *ret_base_virt_addr = region_obj->base_virt_addr;

  return APR_EOK;
}

/* This function assumes ret_base_virt_addr is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_region_base_virt_addr_v2 (
  uint32_t mem_handle,
  cvd_virt_addr_t* ret_base_virt_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
  mmstd_memset(ret_base_virt_addr, 0, sizeof( cvd_virt_addr_t ) );

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_get_region_base_virt_addr_v2(): Invalid " \
            "mem_handle=0x%08X",
            mem_handle );
    return rc;
  }
  cvd_mem_mapper_set_virt_addr( ret_base_virt_addr, region_obj->base_virt_addr );

  return APR_EOK;
}

/* This function assumes ret_size is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_region_size (
  uint32_t mem_handle,
  uint32_t* ret_size
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_get_region_size(): Invalid mem_handle=0x%08X",
            mem_handle );
    return rc;
  }

  *ret_size = region_obj->size;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_validate_handle (
  uint32_t mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_validate_handle(): Invalid mem_handle=0x%08X",
            mem_handle );
    return rc;
  }

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_validate_attributes_align (
  uint32_t mem_handle,
  uint64_t mem_addr
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_validate_attributes_align(): Invalid " \
            "mem_handle=0x%08X",
            mem_handle );
    return rc;
  }

  if ( mem_addr & ( region_obj->access_align - 1 ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_validate_attributes_align(): Mem_addr 0x%08X does " \
            "not meet the alignment requirement %d",
            mem_addr, region_obj->access_align );
    return APR_EFAILED;
  }

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_validate_access_align (
  void* mem_addr,
  uint32_t mem_size,
  uint32_t align
)
{
  if ( ( ( ( uint32_t )mem_addr ) & ( align - 1 ) ) ||
       ( mem_size & ( align - 1 ) )
  )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_validate_access_align(): Mem_addr 0x%08X, size %u " \
            "does not meet the alignment requirement %d",
            mem_addr, mem_size, align );
    return APR_EFAILED;
  }

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_validate_mem_is_in_region (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint32_t mem_size
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
  uint64_t virt_addr;

  for ( ;; )
  {
    rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Invalid " \
              "mem_handle=0x%08X",
              mem_handle );
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr( mem_handle, mem_addr, &virt_addr );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Unable to get the " \
              "virt_addr for mem_handle 0x%08X mem_addr 0x%016X",
              mem_handle, mem_addr );
      break;
    }

    if ( ( virt_addr < region_obj->base_virt_addr ) ||
         ( ( virt_addr + mem_size ) > 
           ( region_obj->base_virt_addr + region_obj->size ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Mem_addr 0x%016X " \
              "is not in the region pointed to by the mem_handle 0x%08X",
              mem_addr, mem_handle );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Corresponding " \
              "virt_addr for the mem_addr is 0x%016X",
              virt_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Starting virt_addr " \
              "of the region pointed to by the mem_handle is 0x%016X",
              region_obj->base_virt_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Size of the region " \
              "pointed to by the mem_handle is %d.",
              region_obj->size );
      break;
    }

    return APR_EOK;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t cvd_mem_mapper_validate_virt_mem_is_in_region (
  uint32_t mem_handle,
  void* mem_ptr,
  uint32_t mem_size
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;
  uint64_t virt_addr;

  for ( ;; )
  {
    rc = cvd_mem_mapper_get_region_object( mem_handle, &region_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Invalid " \
              "mem_handle=0x%08X",
              mem_handle );
      break;
    }
    cvd_mem_mapper_set_virt_addr_to_uint64( &virt_addr, mem_ptr );

    if ( ( virt_addr < region_obj->base_virt_addr ) ||
         ( ( virt_addr + mem_size ) > 
           ( region_obj->base_virt_addr + region_obj->size ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Mem_ptr 0x%08X is " \
              "not in the region pointed to by the mem_handle 0x%08X",
              mem_ptr, mem_handle );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Corresponding " \
              "virt_addr for the mem_ptr is 0x%016X",
              virt_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Starting virt_addr " \
              "of the region pointed to by the mem_handle is 0x%016X",
              region_obj->base_virt_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
              "cvd_mem_mapper_validate_mem_is_in_region(): Size of the region " \
              "pointed to by the mem_handle is %d.",
              region_obj->size );
      break;
    }

    return APR_EOK;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t cvd_mem_mapper_set_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vsm_mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( cvd_mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_set_vsm_mem_handle(): Invalid " \
            "cvd_mem_handle=0x%08X",
            cvd_mem_handle );
    return rc;
  }

  region_obj->vsm_mem_handle = vsm_mem_handle;

  return APR_EOK;
}

APR_INTERNAL int32_t cvd_mem_mapper_set_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vpm_mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( cvd_mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_set_vpm_mem_handle(): Invalid " \
            "cvd_mem_handle=0x%08X",
            cvd_mem_handle );
    return rc;
  }

  region_obj->vpm_mem_handle = vpm_mem_handle;

  return APR_EOK;
}

/* This function assumes ret_vsm_mem_handle is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vsm_mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( cvd_mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_get_vsm_mem_handle(): Invalid " \
            "cvd_mem_handle=0x%08X",
            cvd_mem_handle );
    return rc;
  }

  *ret_vsm_mem_handle = region_obj->vsm_mem_handle;
  return APR_EOK;
}

/* This function assumes ret_vpm_mem_handle is valid. */
APR_INTERNAL int32_t cvd_mem_mapper_get_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vpm_mem_handle
)
{
  int32_t rc;
  cvd_mem_mapper_region_object_t* region_obj;

  rc = cvd_mem_mapper_get_region_object( cvd_mem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   \
            "cvd_mem_mapper_get_vpm_mem_handle(): Invalid " \
            "cvd_mem_handle=0x%08X",
            cvd_mem_handle );
    return rc;
  }

  *ret_vpm_mem_handle = region_obj->vpm_mem_handle;

  return APR_EOK;
}

APR_INTERNAL uint32_t cvd_mem_mapper_obj_stat (
  uint32_t *objects_used
)
{
  if( NULL == objects_used )
  {
    return APR_EBADPARAM;
  }
  *objects_used = CVD_MEM_MAPPER_MAX_OBJECTS_V - cvd_mem_mapper_objmgr.free_q.size;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "cvd_mem_mapper_obj_stat(): Objects used=(%d)",
                                        *objects_used );
  return APR_EOK;
}

