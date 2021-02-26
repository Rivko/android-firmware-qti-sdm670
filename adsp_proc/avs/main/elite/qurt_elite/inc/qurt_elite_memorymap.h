/*========================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */
/**
@file qurt_elite_memorymap.h

@brief This file contains utilities for memory mapping and unmapping of shared
memory, LPM, hardware registers, MIDI etc.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_memory_map_intro
The following functions map and unmap shared memory, LPM, hardware registers,
MIDI, etc.
 -  qurt_elite_memorymap_register()
  - qurt_elite_memorymap_unregister()
  - qurt_elite_memorymap_shm_mem_map()
  - qurt_elite_memorymap_virtaddr_mem_map()
  - qurt_elite_memorymap_get_mapping_mode()
  - qurt_elite_memorymap_shm_mem_unmap()
  - qurt_elite_memorymap_get_virtual_addr_from_shmm_handle()
  - qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle()
  - qurt_elite_memorymap_unmap_all()
  - qurt_elite_memorymap_cache_flush()
  - qurt_elite_memorymap_cache_invalidate()
  - qurt_elite_memorymap_get_physical_addr()
  - qurt_elite_memorymap_shm_incr_refcount()
  - qurt_elite_memorymap_shm_decr_refcount()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_memorymap.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw      (Tech Pubs) General edits in Doxygen comments.
05/11/15   sw      (Tech Pubs) Edited Doxygen hyperlinks for 8952.
09/08/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
02/07/12   kk      Badger API changes 
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/28/11   AZ      Support virtual contiguous memory.
06/26/10   kk      Created file.
========================================================================== */

#ifndef QURT_ELITE_MEMORYMAP_H
#define QURT_ELITE_MEMORYMAP_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_mutex.h"
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"

#if defined(__qdsp6__)
#include "qurt.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_memorymap
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
//***************************************************************************
// QURT_ELITE_MEMORYPOOLTYPE
//***************************************************************************

/** Valid memory map pool IDs.

    In the QuRT operating system, memory map pool IDs are directly referred to
    by pool name.
 */
typedef enum
{
    QURT_ELITE_MEMORYMAP_DEFAULTPHY_POOL=0, /**< Default physical pool. */
    QURT_ELITE_MEMORYMAP_EBI1_POOL,         /**< EBI1 pool. */
    QURT_ELITE_MEMORYMAP_SMI_POOL,          /**< Stacked memory interface pool. */
    QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL,    /**< Shared memory; 8-byte addressable,
                                                 4 KB aligned memory pool. */
    QURT_ELITE_MEMORYMAP_HLOS_PHYSPOOL,     /**< Pool on the DSP side to which
                                                 the HLOS adds or removes pages. */
    QURT_ELITE_MEMORYMAP_POOL_OUT_OF_RANGE  /**< Out of range. */    //keep at end
}  QURT_ELITE_MEMORYPOOLTYPE;

/** Valid memory mapping modes.
*/
typedef enum {
    QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING = 0,
    /**< Physical memory is mapped. */

    QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING
    /**< Virtual memory is mapped. */

} qurt_elite_memorymap_mapping_mode_t;


/** L1 is write-back, and L2 is cached. */
#define HMEM_CACHE_WRITEBACK QURT_MEM_CACHE_WRITEBACK

/** L1 is write-through, and L2 is cached. */
#define HMEM_CACHE_WRITETHROUGH QURT_MEM_CACHE_WRITETHROUGH

/** L1 is write-back, and L2 is uncached. */
#define HMEM_CACHE_WRITEBACK_NONL2CACHEABLE QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE

/** L1 is write-through, and L2 is uncached. */
#define HMEM_CACHE_WRITETHROUGH_NONL2CACHEABLE QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE

/** L1 is write-back, and L2 is cached. */
#define HMEM_CACHE_WRITEBACK_L2CACHEABLE QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE

/** L1 is write-through, and L2 is cached. */
#define HMEM_CACHE_WRITETHROUGH_L2CACHEABLE QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE

/** L1 is uncached, and L2 is an uncached device type. */
#define HMEM_CACHE_NONE QURT_MEM_CACHE_NONE

/** L1 is uncached, and L2 is an uncached shared type. */
#define HMEM_CACHE_NONE_SHARED QURT_MEM_CACHE_NONE_SHARED


/** Wrapper for qurt_mem_cache_mode_t.
  The following options are supported:
  - #HMEM_CACHE_WRITEBACK=7
  - #HMEM_CACHE_WRITETHROUGH=5
  - #HMEM_CACHE_WRITEBACK_NONL2CACHEABLE=0
  - #HMEM_CACHE_WRITETHROUGH_NONL2CACHEABLE=1
  - #HMEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK
  - #HMEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH
  - #HMEM_CACHE_NONE=4
  - #HMEM_CACHE_NONE_SHARED=6
 */
typedef qurt_mem_cache_mode_t qurt_elite_memorymap_cache_mode_t;

/** Contiguous shared memory region, with the start address and size.
 */
typedef struct {
    uint32_t shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. */

    uint32_t shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw word must be
         contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32 bit value.
         @tablebulletend */

    uint32_t mem_size;
    /**< Size of the shared memory region.

         Number of bytes in the shared memory region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. @newpagetable */

} qurt_elite_memorymap_shm_region_t
;


/** Memory mapped region attributes.
 */
typedef struct {
    uint32_t mem_reg_base_phy_addr_lsw;
   /**< Lower 32 bits of the 64-bit memory region start (base) physical
        address. */

    uint32_t mem_reg_base_phy_addr_msw;
   /**< Upper 32 bits of the 64-bitmemory region start (base) physical
        address.

         The 64-bit number formed by mem_reg_base_phy_addr_lsw and
         mem_reg_base_phy_addr_msw word must be contiguous memory, and it must
         be 4 KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value.
         @tablebulletend */

    uint32_t mem_reg_size;
    /**< Size of the shared memory region. 

         Number of bytes in the shared memory region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */

    uint32_t mem_reg_base_virt_addr;
    /**< Memory region start (base) virtual address. */

    uint32_t req_virt_adrr;
    /**< Virtual address that corresponds to the requested physical address. */

    uint32_t rem_reg_size;
    /**< Remaining memory region size from the requested physical address,
         including the requested physical address:

         ([mem_reg_base_phy_addr_msw,mem_reg_base_phy_addr_lsw] +
         mem_reg_size - [requested physical address]) @newpagetable */

} qurt_elite_memorymap_mem_region_attrib_t
;


/**
  Registers a client with qurt_elite_memorymap.

  @param[out] client_token_ptr  Pointer to an instance of
                         qurt_elite_memorymap_client_t that is
                         created and returned as a handle/token to the client.
                         \n
                         This handle uniquely identifies the client, and the
                         client must use this handle for future communication
                         with qurt_elite_memorymap.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None.
*/
ADSPResult qurt_elite_memorymap_register(uint32_t *client_token_ptr);


/**
  Deletes all regions tagged to this client and unregisters this client from
  qurt_elite_memorymap.

  @param[in] client_token  Client token.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered. @newpage
*/
ADSPResult qurt_elite_memorymap_unregister(uint32_t client_token);


/**
  Maps the shared memory and adds the memory region to the client linked list.
  This function enables physical address mapping only.

  @datatypes
  qurt_elite_memorymap_shm_region_t \n
  #qurt_elite_memorymap_cache_mode_t \n
  #QURT_ELITE_MEMORYPOOLTYPE

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to an array of shared memory
                                     regions to map.
  @param[in]  num_shm_reg            Number of shared memory regions in the
                                     array.
  @param[in]  cache_attr             Any HMEM_CACHE attribute.
  @param[in]  pool_id                Memory pool ID to which this region is
                                     mapped.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     shared memory region created. This handle
                                     can be used later to unmap the shared
                                     memory.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered. @newpage
*/
ADSPResult qurt_elite_memorymap_shm_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Maps the shared virtual address and adds the memory region to the client 
  linked list. This function must be used to map a virtual address.

  @datatypes
  qurt_elite_memorymap_shm_region_t \n
  #qurt_elite_memorymap_cache_mode_t \n
  #QURT_ELITE_MEMORYPOOLTYPE

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to an array of shared memory
                                     regions to map.
  @param[in]  num_shm_reg            Number of shared memory regions in the
                                     array.
  @param[in]  cache_attr             Any HMEM_CACHE attribute.
  @param[in]  pool_id                Memory pool ID to which this region is
                                     mapped.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     shared memory region created. This handle
                                     can be used later to unmap the shared
                                     memory.
       
  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered. @newpage
*/
ADSPResult qurt_elite_memorymap_virtaddr_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Gets the memory mapping mode for a specified memory map handle.

  @datatypes
  #qurt_elite_memorymap_mapping_mode_t

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling one of the following:
                                  - qurt_elite_memorymap_shm_mem_map()
                                  - qurt_elite_memorymap_virtaddr_mem_map()
                                  @tablebulletend
  @param[out] mapping_mode_ptr    Pointer to the memory mapping mode.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered. @newpage
*/
ADSPResult qurt_elite_memorymap_get_mapping_mode(    uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    qurt_elite_memorymap_mapping_mode_t* mapping_mode_ptr);

/**
  Unmaps the memory region and deletes the entry from the client-given memory
  map handle.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
    Before calling this function, the client object must be registered, and the
    corresponding memory must be mapped.
*/
ADSPResult qurt_elite_memorymap_shm_mem_unmap(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );

/**
  Gets the corresponding virtual address for a specified shared memory address
  and memory map handle.

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling
                                  qurt_elite_memorymap_shm_mem_map().
  @param[in]  shm_addr_lsw        LSW of the mapped region for any shared
                                  memory address.
  @param[in]  shm_addr_msw        MSW of the mapped region for any shared
                                  memory address.
  @param[out] virt_addr_ptr       Pointer to the equivalent virtual address
                                  returned.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped. @newpage
*/
ADSPResult qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    uint32_t *virt_addr_ptr);

/**
  Gets the region attributes for a specified shared memory address and memory
  map handle.

  @datatypes
  qurt_elite_memorymap_mem_region_attrib_t

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling
                                  qurt_elite_memorymap_shm_mem_map().
  @param[in]  shm_addr_lsw        LSW of the mapped region for any shared
                                  memory address.
  @param[in]  shm_addr_msw        MSW of the mapped region for any shared
                                  memory address.
  @param[out] mem_reg_attrib_ptr  Pointer to the memory region attribute whose
                                  fields are filled by this function.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped.
*/
ADSPResult qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    qurt_elite_memorymap_mem_region_attrib_t *mem_reg_attrib_ptr);

/**
  Unmaps all memory regions and deletes all nodes of the requested client.

  @param[in] client_token  Client token.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered. @newpage
*/
ADSPResult qurt_elite_memorymap_unmap_all(uint32_t client_token);

/**
  Flushes the cache (data) of the specified memory region.

  @param[in] virt_addr  Starting virtual address.
  @param[in] mem_size   Size of the region to flush.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped.
*/
ADSPResult qurt_elite_memorymap_cache_flush(uint32_t virt_addr,
                      uint32_t mem_size);

/**
  Invalidates the cache (data) of the specified memory region.

  @param[in]  virt_addr  Starting virtual address.
  @param[in]  mem_size   Size of the region to invalidate.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped. @newpage
*/
ADSPResult qurt_elite_memorymap_cache_invalidate(uint32_t virt_addr,
                       uint32_t mem_size);

/**
  Given a virtual address, this function does the following:
  - Retrieves the corresponding mapping entry from the page table
  - Adjusts the offset based on the page size
  - Returns the full 64-bit physical address back to the user

  @param[in] virt_addr    Starting virtual address.

  @return
  Physical address if the address is mapped; otherwise, 0.

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped.
*/
uint64_t qurt_elite_memorymap_get_physical_addr(uint32_t virt_addr);

/**
  Increments the reference count of the memory map handle of a client.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @detdesc
  Incrementing this reference count suggests that the memory region abstracted
  by this memory map handle is in use.
  @par
  A nonzero reference count prevents the aDSP client from unmapping specific
  memory map regions.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped. @newpage
*/
ADSPResult qurt_elite_memorymap_shm_incr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );

/**
  Decrements the reference count of the memory map handle of a client.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @detdesc
  Decrementing this reference count suggests that this client is relinquishing
  the memory region abstracted by this memory map handle (the client no longer
  requires this region).
  @par
  The reference count must reach zero for the memory region to be unmapped.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the client object must be registered, and the
  corresponding memory must be mapped.
*/
ADSPResult qurt_elite_memorymap_shm_decr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );

/**
  Adds a page to or removes a page from a physical memory pool.

  @datatypes
  QURT_ELITE_MEMORYPOOLTYPE

  @param[in] is_add    Specifies whether to add or remove a page.
  @param[in] pool_id   ID of the pool.
  @param[in] size      Size of the page.
  @param[in] phy_addr  Physical address of the page.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  If is_add = FALSE -- Before calling this function, a page must be added.
  If is_add = TRUE -- There is no dependency.
*/
ADSPResult qurt_elite_memorymap_add_remove_pool_page(bool_t is_add,
      QURT_ELITE_MEMORYPOOLTYPE pool_id, uint32_t size, uint64_t phy_addr);

/** @} */ /* end_addtogroup qurt_elite_memorymap */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MEMORYMAP_H

