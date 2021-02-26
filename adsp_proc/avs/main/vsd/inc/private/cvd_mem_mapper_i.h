#ifndef __CVD_MEM_MAPPER_I_H__
#define __CVD_MEM_MAPPER_I_H__

/*
   Copyright (c) 2010-2011, 2012, 2013, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_mem_mapper_i.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "vss_imemory_public_if.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_MEM_MAPPER_SUPPORTED_CACHE_LINE_SIZE ( 128 )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef struct cvd_addr64_t
{
   uint32_t word0;
   /**< Lower word */
   uint32_t word1;
   /**< Upper word */
}cvd_addr64_t;

typedef union cvd_virt_addr_t
{
   cvd_addr64_t addr64;
   void* ptr;
}cvd_virt_addr_t;

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

/** 
  Initialize the CVD memory mapper. Must be called before any other memory
  mapper APIs can be called, and must be called only once.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_init ( void );

/** 
  Deinitialize the shared memory driver.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_deinit ( void );

/** 
  Invalidate the cache at specified address with specified size.

  @param[in] virt_addr  Address to invalidate.
  @param[in] size       Size to invalidate.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_cache_invalidate (
  uint64_t virt_addr,
  uint32_t size
);

/**
  Invalidate the cache at specified address with specified size.

  @param[in] virt_addr  Address to invalidate.
  @param[in] size       Size to invalidate.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_cache_invalidate_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
);

/**
  Flush the cache at specified address with specified size.

  @param[in] virt_addr  Address to Flush.
  @param[in] size       Size to Flush.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_cache_flush (
  uint64_t virt_addr,
  uint32_t size
);

/**
  Flush the cache at specified address with specified size.

  @param[in] virt_addr  Address to Flush.
  @param[in] size       Size to Flush.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_cache_flush_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
);

/**
  Maps virtual shared memory.

  @param[in] map_virtual_arg Shared memory information. Refer to 
                             vss_imemory_cmd_map_virtual_t in 
                             vss_imemory_public_if.h for details.
  @param[out] ret_mem_handle Returns handle to the shared memory.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_map_virtual (
  vss_imemory_cmd_map_virtual_t* map_virtual_arg,
  uint32_t* ret_mem_handle
);

/**
  Maps physical shared memory.

  @param[in] map_physical_arg Shared memory information. Refer to 
                              vss_imemory_cmd_map_physical_t in 
                              vss_imemory_public_if.h for details.
  @param[out] ret_mem_handle Returns handle to the shared memory.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_map_physical (
  vss_imemory_cmd_map_physical_t* map_physical_arg,
  uint32_t* ret_mem_handle
);

/**
  Unmap shared memory.

  @param[in] mem_handle  Handle to the shared memory.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_unmap (
  uint32_t mem_handle
);

/**
  Retrieve virtual address given virtual address, physical address or address
  offset and memory handle.

  @param[in] mem_handle     Handle to the shared memory.
  @param[in] mem_addr       Memory address (virtual, physical or offset).
  @param[out] ret_virt_addr  Returns virtual memory address.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_virtual_addr (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint64_t* ret_virt_addr
);

/**
  Retrieve virtual address given virtual address, physical address or address
  offset and memory handle.

  @param[in] mem_handle     Handle to the shared memory.
  @param[in] mem_addr       Memory address (virtual, physical or offset).
  @param[out] ret_virt_addr  Returns virtual memory address.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_virtual_addr_v2 (
  uint32_t mem_handle,
  uint64_t mem_addr,
  cvd_virt_addr_t* ret_virt_addr
);

/**
  Retrieve virtual address, physical address or address offset given the 
  virtual address and memory handle. 

  @param[in] mem_handle    Handle to the shared memory.
  @param[in] virt_addr     Virtual address.
  @param[in] ret_mem_addr  Returns virtual address, physical address or address
                           offset based on the memory attributes.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_mem_addr (
  uint32_t mem_handle,
  uint64_t virt_addr,
  uint64_t* ret_mem_addr
);


/**
  Retrieve base virtual address of a shared memory region.

  @param[in] mem_handle           Handle to the shared memory.
  @param[out] ret_base_virt_addr  Returns base virtual address for the shared
                                  memory region.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_region_base_virt_addr (
  uint32_t mem_handle,
  uint64_t* ret_base_virt_addr
);

/**
  Retrieve base virtual address of a shared memory region.

  @param[in] mem_handle           Handle to the shared memory.
  @param[out] ret_base_virt_addr  Returns base virtual address for the shared
                                 memory region.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_region_base_virt_addr_v2 (
  uint32_t mem_handle,
  cvd_virt_addr_t* ret_base_virt_addr
);

/**
  Retrieve the size in bytes of a shared memory region.

  @param[in] mem_handle  Handle to the shared memory.
  @param[in] ret_size    Returns size in bytes of the shared memory region.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_region_size (
  uint32_t mem_handle,
  uint32_t* ret_size
);

/**
  Validate the memory handle is valid.

  @param[in] mem_handle  Handle to the shared memory.

  @return APR_EOK if mem_addr meets the alignment requirement.
  @return APR_EFAILED if mem_addr doesn't meet the alignment requirement.
*/
APR_INTERNAL int32_t cvd_mem_mapper_validate_handle (
  uint32_t mem_handle
);

/**
  Validate the memory access alignment based on shared memory attributes 
  encapusalated by the memory handle.

  @param[in] mem_handle  Handle to the shared memory.
  @param[in] mem_addr    Memory address (virtual, physical or offset).

  @return APR_EOK if mem_addr meets the alignment requirement.
  @return APR_EFAILED if mem_addr doesn't meet the alignment requirement.
*/
APR_INTERNAL int32_t cvd_mem_mapper_validate_attributes_align (
  uint32_t mem_handle,
  uint64_t mem_addr
);

/**
  Verifies the alignment of the pointer based on supplied alignment
  and checks if size is a multiple of the type width.

  @param[in] mem_addr    Memory address (virtual, physical or offset).
  @param[in] mem_size    Memory size.
  @param[in] align       Required alignment, needs to be a power of 2.

  @return APR_EOK if mem_addr meets the alignment requirement.
  @return APR_EFAILED if mem_addr doesn't meet the alignment requirement.
*/
APR_INTERNAL int32_t cvd_mem_mapper_validate_access_align (
  void* mem_addr,
  uint32_t mem_size,
  uint32_t align
);

/**
  Validate the memroy specified by the address and size is within the region 
  pointed to by the memory handle.

  @param[in] mem_handle  Handle to the shared memory.
  @param[in] mem_addr    Memory address (virtual, physical or offset).
  @param[in] mem_size    Memory size in bytes.

  @return APR_EOK if memory is within region.
  @return APR_EFAILED if memory is not within region.
*/
APR_INTERNAL int32_t cvd_mem_mapper_validate_mem_is_in_region (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint32_t mem_size
);

/**
  Validate the virtual address and size specified is within the region 
  pointed to by the memory handle.

  @param[in] mem_handle  Handle to the shared memory.
  @param[in] mem_addr    Memory address.
  @param[in] mem_size    Memory size in bytes.

  @return APR_EOK if memory is within region.
  @return APR_EFAILED if memory is not within region.
*/
APR_INTERNAL int32_t cvd_mem_mapper_validate_virt_mem_is_in_region (
  uint32_t mem_handle,
  void* mem_addr,
  uint32_t mem_size
);

/**
  Sets the vsm memory handle associated with the cvd memory handle.

  @param[in] cvd_mem_handle  CVD memory handle.
  @param[in] vsm_mem_handle  VSM memory handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_set_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vsm_mem_handle
);

/**
  Sets the vpm memory handle associated with the cvd memory handle.

  @param[in] cvd_mem_handle  CVD memory handle.
  @param[in] vpm_mem_handle  VPM memory handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_set_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vpm_mem_handle
);

/**
  Retrieves the vsm memory handle associated with the cvd memory handle.

  @param[in] cvd_mem_handle       CVD memory handle.
  @param[out] ret_vsm_mem_handle  Returns the VSM memory handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vsm_mem_handle
);

/**
  Retrieves the vpm memory handle associated with the cvd memory handle.

  @param[in] cvd_mem_handle       CVD memory handle.
  @param[out] ret_vsm_mem_handle  Returns the VPM memory handle.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_mem_mapper_get_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vpm_mem_handle
);

/****************************************************************************
 * INLINE FUNCTIONS                                                         *
 ****************************************************************************/

/**
  Populates a pointer from a 64 bit integer.
  @param[out] ptr_dest       Pointer that is populated.
  @param[in] virt_addr_src   Source virtual address.
*/
static inline void cvd_mem_mapper_set_uint64_to_virt_addr( void **ptr_dest, uint64_t virt_addr_src )
{
  *ptr_dest = ( void* )( ( uint32_t )virt_addr_src );
  return;
}

/**
  Populates a cvd_virt_addr_t structure from a pointer
  @param[out] virt_addr_dest  Structure that is populated.
  @param[in] virt_addr_src    Source virtual address.
*/
static inline void cvd_mem_mapper_set_virt_addr( cvd_virt_addr_t *virt_addr_dest, uint64_t virt_addr_src )
{
  cvd_mem_mapper_set_uint64_to_virt_addr( &virt_addr_dest->ptr, virt_addr_src );
  return;
}

/**
  Populates a pair of 32 bit integers from a pointer.
  @param[out] msw_dest       Upper word that is populated.
  @param[out] lsw_dest       Lower word that is populated.
  @param[in] virt_addr_src  Source virtual address.
*/
static inline void cvd_mem_mapper_set_virt_addr_to_uint32( uint32_t *msw_dest, uint32_t *lsw_dest, void *ptr_src )
{
  *lsw_dest = ( uint32_t )ptr_src;
  *msw_dest = 0;
  return;
}

/**
  Populates a 64 bit integer from a pointer.
  @param[out] virt_addr_dest  Integer that is populated.
  @param[in] ptr_src          Source virtual address.
*/
static inline void cvd_mem_mapper_set_virt_addr_to_uint64( uint64_t *virt_addr_dest, void *ptr_src )
{
  *virt_addr_dest = ( uint64_t )( ( uint32_t )ptr_src );
  return;
}

/**
  Retrives the number of active apr objects used for MEM MAPPER heap at that time of query.

  @param[out] objects_used - number of active objects 

  @return APR_EOK when successful.
*/
APR_INTERNAL uint32_t cvd_mem_mapper_obj_stat (
  uint32_t *objects_used
);

#endif /* __CVD_MEMORY_MAPPER_I_H__ */

