#ifndef ELITE_LIB_MEM_MAPPER_H
#define ELITE_LIB_MEM_MAPPER_H

/**
  @file Elite_lib_mem_mapper.h

  @brief A CAPIv2 supporting library for a memory mapper
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/* =========================================================================
 Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 Export of this technology or software is regulated by the U.S. Government,
 Diversion contrary to U.S. law prohibited.
  ========================================================================== */
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_lib_mem_mapper.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  08/28/17    sw       (Tech Pubs) Edited Doxygen comments for AVS.2.8 (SDM845).
  2/11/2017    rbhatnk   Created file.
=============================================================================*/

/** @weakgroup weakf_capiv2_lib_mem_map_intro

The Memory Mapper library can be used to map and unmap memory buffers that are
shared by another subsystem such as the HLOS.

This utility exposes functions that are used to do the following:
- Map physical memory to virtual memory
- Get the virtual memory for a specified address in the mapped range
- Unmap the physical memory
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "adsp_audio_memmap_api.h"


/** @addtogroup capiv2_lib_mem_map
@{ */

/** Unique identifier to represent this library interface.*/
#define ELITE_LIB_MEM_MAPPER 0x0001321B

/*------------------------------------------------------------------------------
 * Interface definition
 *----------------------------------------------------------------------------*/

typedef struct elite_lib_mem_mapper_t elite_lib_mem_mapper_t;

/** Virtual function table of the memory mapper interface.

  To use this library:
   -# Get an object of type elite_lib_get_mem_mapper_t using
      #ELITE_LIB_MEM_MAPPER.
   -# Use map_memory to map any physical memory regions to virtual memory.
   -# Use get_virt_addr to get the virtual address of specified physical memory.
   -# Use unmap_memory to remove the mapping of physical memory to virtual memory.
 */
struct elite_lib_mem_mapper_vtable
{
   capi_v2_library_base_t b;
   /**< Maps to the base interface for the library. */

   /**
     Maps the shared memory provided by the HLOS.

     @datatypes
     elite_lib_mem_mapper_t

     @param[in] obj_ptr         Pointer to the instance of this object.
     @param[in] mem_pool_id     Memory pool ID passed by the HLOS.
     @param[in] property_flag   Property flag of the memory passed by the HLOS.
     @param[in] is_cached       Specifies whether to map the memory as cached. \n
                                @values
                                - TRUE -- Cached
                                - FALSE -- Not cached @tablebulletend
     @param[in] num_regions     Elements in the regions_array.
     @param[in] regions_array   Array of regions to be mapped into a virtual
                                space.
     @param[in] mem_map_handle  Handle to the memory mapping.

     @return
     Memory map handle.

     @dependencies
     None. @newpage
    */
   capi_v2_err_t (*map_memory)(elite_lib_mem_mapper_t *obj_ptr,
         uint16_t mem_pool_id, uint16_t property_flag, bool_t is_cached,
         uint16_t num_regions, avs_shared_map_region_payload_t *regions_array,
         uint32_t *mem_map_handle);

   /**
     Unmaps the memory.

     @datatypes
     elite_lib_mem_mapper_t

     @param[in] obj_ptr         Pointer to the instance of this object.
     @param[in] mem_map_handle  Handle of the memory mapping returned by
                                map_memory().

     @return
     CAPIV2_EOK -- Success \n
     capi_v2_err_t -- Failure (see Section @xref{dox:errorCodes})

     @dependencies
     The memory region must have been previously mapped using map_memory().
     */
   capi_v2_err_t (*unmap_memory)(elite_lib_mem_mapper_t *obj_ptr, uint32_t mem_map_handle);


   /**
     Gets the virtual address of the mapped memory region.

     @datatypes
     elite_lib_mem_mapper_t \n

     @param[in]  obj_ptr         Pointer to the instance of this object.
     @param[in]  mem_map_handle  Handle of the memory mapping returned by
                                 map_memory().
     @param[in]  phy_addr_lsw    LSW of the physical address to be unmapped.
     @param[in]  phy_addr_lsw    MSW of the physical address to be unmapped.
     @param[out] virt_addr       Virtual address that corresponds to the
                                 physical address.

     @return
     Virtual address.

     @dependencies
     The memory region must have been previously mapped using map_memory().
     @newpage
    */
   capi_v2_err_t (*get_virt_addr)(elite_lib_mem_mapper_t *obj_ptr, uint32_t mem_map_handle,
            uint32_t phy_addr_lsw, uint32_t phy_addr_msw, uint32_t *virt_addr);
};


/** Contains a pointer to the CAPIv2 virtual function table
    defined in elite_lib_mem_mapper_vtable.
 */
struct elite_lib_mem_mapper_t
{
   const elite_lib_mem_mapper_vtable *vtable;
   /**< Pointer to the virtual function table. */
};

/** @} *//* end_addtogroup capiv2_lib_mem_map */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_LIB_MEM_MAPPER_H */
