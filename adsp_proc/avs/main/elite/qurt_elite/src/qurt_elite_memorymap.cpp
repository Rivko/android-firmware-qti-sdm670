/*========================================================================
qurt_elite_memorymap.cpp

This file contains a utility for memory mapping and unmapping shared memory, LPM etc.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_memorymap.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/26/10   kk      Created file.
03/28/11   AZ      Support virtual contiguous memory.
03/01/12   kk      Badger api changes
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#ifndef MDSPMODE
#include "q6_memory_defns.h"
#endif
#include <stringl/stringl.h>
#include "qurt_elite_qdi_driver.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
//#define DEBUG_QURT_ELITE_MEMORYMAP
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

const char qurt_elite_memorymap_pool_name[][32] = {
    {"DEFAULT_PHYSPOOL"},
    {"EBI1_pool"},
    {"SMI_POOL"},
    {"EBI1_pool"}, //Default shared memory pool for SHMEM8_4K
    {"HLOS_PHYSPOOL"}
};


typedef union {
    struct {
        uint32_t        lsw;    /** Starting memory address(LSW) of this region. */
        uint32_t        msw;    /** Starting memory address(MSW) of this region. */
    }mem_addr_32b;

    uint64_t mem_addr_64b;      /** Starting memory address 64bits of this region. */
}mem_addr_t;

/** Defines qurt_elite_memorymap_region_t, which is a wrapper for
    qurt_mem_region_t.
 */
typedef qurt_mem_region_t qurt_elite_memorymap_region_t;

/**
@brief One contiguous shared memory region mapped to virtual *
 *     address. This struct immediately follows the
 *     qurt_elite_memorymap_node_t.
 *
 */
typedef struct {
    mem_addr_t      shm_addr;     /** shared memory address. This address has to be 64bit aligned **/
    qurt_elite_memorymap_region_t   mem_reg;        /** Actual region that will be created. */
    uint32_t        virt_addr;    /** Corresponding starting virtual address of
                                       the region. */
    uint32_t        mem_size;     /** Size of the region.  */
    uint32_t        double_word_padding;  /** padding to make sure the next region record is also 64 bit aligned */

} qurt_elite_memorymap_region_record_t
;

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/**
 *This function creates a memory region using QURT APIs. It
 *does not add to the linked list.
 *
 *It can create 3 types of regions:
 *
 *1. Master region.
 *Indicated by CREATE_MASTER_REGION.
 *It only create a virtual region to reserve some virtual space,
 *no physical memory is involved. After return, reg_ptr->mem_size
 *[in] of virtual memory space is reserved, starting virt address will be
 *stored in reg_ptr->virt_addr [out]. mem_pool_ptr [out] is updated.
 *
 *
 *2. Sub region of a master region.
 *Indicated by MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED.
 *After return, reg_ptr->shm_addr_lsw [in] is mapped to
 *reg_ptr->virt_addr [in], created region has reg_ptr->mem_size
 *[in], mem_pool_ptr [in] is used.
 *
 *
 *3. Stand alone region.
 *Indicated by MAP_PHYSICAL_REGION_TO_VIRTUAL_ANY.
 *After return, reg_ptr->shm_addr_lsw [in] is mapped to any available
 *virtual address which will be stored in reg_ptr->virt_addr
 *[out], created region has reg_ptr->mem_size [in], mem_pool [out]
 *is updated.
 */

typedef enum {
   CREATE_MASTER_REGION  =    0,
   MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED  ,
   MAP_PHYSICAL_REGION_TO_VIRTUAL_ANY
} qurt_elite_memorymap_command;

static ADSPResult qurt_elite_memorymap_region_create(
                  qurt_elite_memorymap_region_record_t * reg_ptr, //[in/out] hold info for creating a qurt region.
                  qurt_elite_memorymap_cache_mode_t cache_attr, // [in] Cache attributes.
                  QURT_ELITE_MEMORYPOOLTYPE pool_id, // [in] Pool ID.
                  qurt_mem_pool_t*     mem_pool_ptr, // [in/out] Qurt pool object, [in] when create sub regions
                                            // from reserved master region; [out] when create other regions.
                  qurt_elite_memorymap_command create_cmd // [in] Indicate the kind of region to create.
       );





/**
 *This function deletes a memory region using QURT APIs. This
 *function is not thread safe. The caller must provide thread
 *safety.
*/
static ADSPResult qurt_elite_memorymap_region_delete(qurt_elite_memorymap_node_t *mem_map_node_ptr);

/**
 *This function verifies if a given client is already registered
 *with the qurt_elite_memorymap or not.
*/
static ADSPResult qurt_elite_memorymap_client_find(uint32_t client_token);

/**
 *This function verifes if a given physical address is in the
 *valid range of the given memory map pool.
*/
static ADSPResult qurt_elite_memorymap_verify_pool_range(uint32_t shm_addr,uint32_t mem_size, QURT_ELITE_MEMORYPOOLTYPE *pool_id_ptr);

/**
 *This function updates the ref count of a memory map handle
*/

static ADSPResult qurt_elite_memorymap_shm_update_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    int16_t count_value
                    );

/**
Maps the shared memory and but does not add the memory region to
the client linked list.

@param[in] client_token Client token.
@param[in] shm_mem_reg_ptr Array of shared memory regions to
      map.
@param[in] num_shm_reg Number of shared memory regions in the
      array.
@param[in] cache_attr Can be any of HMEM_CACHE attributes.
@param[in] pool_id Specifies the memory pool ID to which this
       region is to be mapped.
@param[out] shm_mem_map_handle_ptr The memory map handle of the shared memory
        region created. This handle can later be used to unmap the shared
        memory


@return
 ADSPResult error code.

@dependencies
 The client object must be registered before calling this
 function.
*/
static ADSPResult qurt_elite_memorymap_shm_phymem_map(    uint32_t client_token,
                                                          qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                                                          uint16_t num_shm_reg,
                                                          qurt_elite_memorymap_cache_mode_t cache_attr,
                                                          QURT_ELITE_MEMORYPOOLTYPE pool_id,
                                                          uint32_t* shm_mem_map_handle_ptr);


/**
 *This function adds the memory map node to the client list and
 *appropriately update the qurt_elite_globalstate .
*/
static void qurt_elite_memorymap_add_mem_map_node_to_client(uint32_t client_token,
                                                                  qurt_elite_memorymap_node_t *mem_map_node_ptr
                                                                  );


/****************************************************************************
** Memory Map
*****************************************************************************/




ADSPResult qurt_elite_memorymap_register(uint32_t *client_token_ptr)
{

    qurt_elite_memorymap_client_t *client_ptr;

    // allocate space for client
    if (NULL == (client_ptr = (qurt_elite_memorymap_client_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_memorymap_client_t), QURT_ELITE_HEAP_DEFAULT)))
    {
       return ADSP_ENOMEMORY;
    }

    //initialize the client.
    QURT_ELITE_ZEROAT(client_ptr);

    // initialize mutex
    qurt_elite_mutex_init(&(client_ptr->mClientMutex));

    // add to global debug structure
    qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
    client_ptr->pNext = qurt_elite_globalstate.pMemoryMapClientList;
    qurt_elite_globalstate.pMemoryMapClientList = client_ptr;
    qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

    //return the client token to the client for future communication
    *client_token_ptr = (uint32_t)client_ptr;


    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap client registered and the client token is 0x%x", (unsigned int)*client_token_ptr);

    return ADSP_EOK;
}

ADSPResult qurt_elite_memorymap_shm_phymem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr)
{

    ADSPResult rc;
    QURT_ELITE_MEMORYPOOLTYPE temp_mem_pool_id=pool_id, mem_pool_id = pool_id;

    //initialize the memory map handle
    *shm_mem_map_handle_ptr = 0;
    /************************** Validating input parameters ********************************/
    /************************** Validating input parameters ********************************/

    //check if number of shared mem regions is non zero
    if (0 >= num_shm_reg)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_elite_memorymap invalid num of shm regions %d",num_shm_reg);
        return ADSP_EBADPARAM;
    }

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token)))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap cannot find the (client token,status) 0x%x 0x%x)", (unsigned int)client_token,(unsigned int)rc);
        return rc;
    }

    // Verify all shared memory address regions are from the same pool
    // check if region size is a multiple of 4K
    // check if each region start address is aligned to 4K
    for (int i=0; i<num_shm_reg; ++i)
    {
         //each region size and start addr must be a multiple of 4K, otherwise return an error
         if ((shm_mem_reg_ptr[i].mem_size & 0xFFF) || (shm_mem_reg_ptr[i].shm_addr_lsw & 0xFFF))
         {
             rc = ADSP_EBADPARAM;
             MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap either region size or start addr is not a multiple of 4K (addr,size,status)->(0x%x,0x%x,0x%x)",
             (unsigned int)shm_mem_reg_ptr[i].shm_addr_lsw, (unsigned int)shm_mem_reg_ptr[i].mem_size, rc);
             return rc;
         }

        if(ADSP_EOK != (rc = qurt_elite_memorymap_verify_pool_range(shm_mem_reg_ptr[i].shm_addr_lsw ,shm_mem_reg_ptr[i].mem_size,&mem_pool_id) ) )
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap verify pool range failed for (addr,size,status)->(0x%x,0x%x,0x%x)",
                    (unsigned int)shm_mem_reg_ptr[i].shm_addr_lsw,(unsigned int)shm_mem_reg_ptr[i].mem_size,rc);
            return rc;
        }

        // check if all regions are from the same pool. mem_pool_id could be changed by qurt_elite_memorymap_verify_pool_range()
        if (0==i)
        {
            temp_mem_pool_id = mem_pool_id;
        }else {
            if (temp_mem_pool_id != mem_pool_id) {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " verify_pool_range failed since not all regions are from the same pool (%d, %d)", temp_mem_pool_id, mem_pool_id);
                return ADSP_EBADPARAM;
            }
        }
    }

    // here, all the input paramters to this func looks good. create an qurt elite entry, and do the qurt mapping
    // Allocate a node for this mapping (the global linklist for a client's mapped regions is
    // organized as one-node-per-invocation.

    // Pointer to the first phys region record in the node.
    qurt_elite_memorymap_region_record_t * cont_phys_regions_ptr;

    // Determine the size of the node, depending the number of phys regions in the invocation.
    // one extra entry for master (qurt virtual mem) region which is kept at the last.
    uint32_t total_node_alloc_size = sizeof(qurt_elite_memorymap_node_t) + (sizeof(qurt_elite_memorymap_region_record_t) * (num_shm_reg + 1));

    // Allocate node
    qurt_elite_memorymap_node_t* mem_map_node_ptr;
    if ((NULL == (mem_map_node_ptr =(qurt_elite_memorymap_node_t *) qurt_elite_memory_aligned_malloc(
        total_node_alloc_size,8,
        QURT_ELITE_HEAP_DEFAULT)) ))
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap fail to allocate memory for the memory map node");
        return ADSP_ENOMEMORY;
    }else {
        // Initialize the node
        memset(mem_map_node_ptr, 0, total_node_alloc_size);
        mem_map_node_ptr->unNumContPhysReg = num_shm_reg;
        mem_map_node_ptr->MemPool = mem_pool_id;
        // Bookmark the first phys region:
        cont_phys_regions_ptr = (qurt_elite_memorymap_region_record_t*) ((uint8_t*)mem_map_node_ptr + sizeof(qurt_elite_memorymap_node_t));
        // Copy over each phys addr and size to the record in the node.
        for (int i=0; i<num_shm_reg; ++i) {
            cont_phys_regions_ptr[i].shm_addr.mem_addr_32b.lsw = shm_mem_reg_ptr[i].shm_addr_lsw;
            cont_phys_regions_ptr[i].shm_addr.mem_addr_32b.msw = shm_mem_reg_ptr[i].shm_addr_msw;
            cont_phys_regions_ptr[i].mem_size = shm_mem_reg_ptr[i].mem_size;

            // If more than or equal to 1 phys region, need map to virtual contiguous memory.
            // Virtual contiguous, calculate total virtual space needed as master region.
            // At this point region size is assumed to be 4K multiple.
            // Also calculate for each region its specific virt addr which is an offset from the starting
            // of the virt addr of the master region.
            // The last region is for master region, accumulate and save the total size needed there,
            cont_phys_regions_ptr[num_shm_reg].mem_size += (cont_phys_regions_ptr[i].mem_size);

        }
    }


     // Create master region, store it in the last slot in the array.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_region_create(
                                 &cont_phys_regions_ptr[num_shm_reg],
                                 cache_attr,
                                 mem_pool_id,
                                 &mem_map_node_ptr->MemPool,
                                 CREATE_MASTER_REGION )))
    {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_elite_memorymap Create Master Region Failed (Status,0x%x)", rc);
         qurt_elite_memory_aligned_free(mem_map_node_ptr);
         return rc;
    }
    // Now the start virt addr of master region, determine each sub region's start virt addr.
    uint32_t curr_virt_addr = cont_phys_regions_ptr[num_shm_reg].virt_addr;
    for (int i = 0; i < num_shm_reg; ++i) {
         cont_phys_regions_ptr[i].virt_addr = curr_virt_addr;
         curr_virt_addr += (cont_phys_regions_ptr[i].mem_size);
    }
    // Create each sub region.
    for (int i=0; i<num_shm_reg; ++i) {
         if (ADSP_EOK != (rc = qurt_elite_memorymap_region_create(
                                     &cont_phys_regions_ptr[i],
                                     cache_attr,
                                     mem_pool_id,
                                     &mem_map_node_ptr->MemPool,
                                     MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED )
                          )
             ){
             MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_elite_memorymap Create shared mem region Failed (Status,0x%x)", rc);
             // Revert what has been mapped so far.
             for (int j=0; j<i; ++j) {
                 qurt_mem_region_delete(cont_phys_regions_ptr[j].mem_reg);
             }
             // Delete master region too.
             qurt_mem_region_delete(cont_phys_regions_ptr[num_shm_reg].mem_reg);
             // Free the node.
             qurt_elite_memory_aligned_free(mem_map_node_ptr);
             return rc;
         }
    }

    // next, sort and add node to list.


    // Sort (bubble) the phys regions in assending order, within the node, for faster phys->virt lookup.
    for(int i=0;i<(num_shm_reg-1);++i)  {
       for(int j=0;j<(num_shm_reg-(i+1));++j)  {
           if(cont_phys_regions_ptr[j].shm_addr.mem_addr_64b > cont_phys_regions_ptr[j+1].shm_addr.mem_addr_64b) {
               qurt_elite_memorymap_region_record_t tmp = cont_phys_regions_ptr[j];
               cont_phys_regions_ptr[j] = cont_phys_regions_ptr[j+1];
               cont_phys_regions_ptr[j+1] = tmp;
           }
       }
    }


    //return mem map handle pointer
    *shm_mem_map_handle_ptr = (uint32_t)mem_map_node_ptr;

    return ADSP_EOK;
}

ADSPResult qurt_elite_memorymap_shm_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr)
{

    ADSPResult rc;
    if (ADSP_EOK != (rc = qurt_elite_memorymap_shm_phymem_map(client_token,shm_mem_reg_ptr,num_shm_reg,
                                             cache_attr,pool_id,shm_mem_map_handle_ptr)))
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_shm_phymem_map failed");
        return rc;
    }



    qurt_elite_memorymap_node_t* mem_map_node_ptr = (qurt_elite_memorymap_node_t*)(*shm_mem_map_handle_ptr);
    //set the mapping type as physical
    mem_map_node_ptr->mapping_mode = QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING;

    //add this memory map to the client
    qurt_elite_memorymap_add_mem_map_node_to_client(client_token,mem_map_node_ptr);

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap successfully mapped all regions and added to client's list (token,mmhandle,num of regions) = (0x%x,0x%x,%d)",
            (unsigned int)client_token,(unsigned int)mem_map_node_ptr, num_shm_reg);
    #endif

    return ADSP_EOK;
}

ADSPResult qurt_elite_memorymap_virtaddr_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr)
{

    ADSPResult rc;
    QURT_ELITE_MEMORYPOOLTYPE mem_pool_id = pool_id;

    //initialize the memory map handle
    *shm_mem_map_handle_ptr = 0;
    /************************** Validating input parameters ********************************/

    //check if number of shared mem regions is non zero
    if (0 >= num_shm_reg)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_elite_memorymap invalid num of shm regions %d",num_shm_reg);
        return ADSP_EBADPARAM;
    }

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token)))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap cannot find the (client token,status) 0x%x 0x%x)", (unsigned int)client_token,rc);
        return rc;
    }

    //for virt addr mapping, assumption is the memory is already mapped and hence cache_attr and pool id is a dont care.
    // make sure virt addr of each region is mapped before and if not return an error.
    // Since it is virt addr, shm_mem_reg_ptr[i].shm_addr_lsw is non zero and shm_mem_reg_ptr[i].shm_addr_msw must be zero.
    for (int i=0; i<num_shm_reg; ++i)
    {
        //each region size must be a multiple of 4K, otherwise return an error
        if ((shm_mem_reg_ptr[i].mem_size & 0xFFF))
        {
            rc = ADSP_EBADPARAM;
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap virt addr mapping failed since region size is not a multiple of 4K (addr,size,status)->(0x%x,0x%x,0x%x)",
                    (unsigned int)shm_mem_reg_ptr[i].shm_addr_lsw,(unsigned int)shm_mem_reg_ptr[i].mem_size,rc);
            return rc;
        }

        if ((0 != shm_mem_reg_ptr[i].shm_addr_msw) ||
            (0 == qurt_elite_memorymap_get_physical_addr(shm_mem_reg_ptr[i].shm_addr_lsw)) ||
            (0 == qurt_elite_memorymap_get_physical_addr(shm_mem_reg_ptr[i].shm_addr_lsw + shm_mem_reg_ptr[i].mem_size -1))
            )
            {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "qurt_elite_memorymap virt addr mapping failed since the corresponding phy cannot be found (client token,virt addr) 0x%x, 0x%x)",
                (unsigned int)client_token,(unsigned int)shm_mem_reg_ptr[i].shm_addr_lsw);
                return ADSP_EFAILED;
            }
    }

    // here, all the input paramters to this func looks good. create an qurt elite entry, but do not do the qurt mapping.

    // Allocate a node for this mapping (the global linklist for a client's mapped regions is
    // organized as one-node-per-invocation.
    // Pointer to the first phys region record in the node.
    qurt_elite_memorymap_region_record_t * cont_phys_regions_ptr;

    // Determine the size of the node, depending the number of phys regions in the invocation.
    // one extra entry for master (qurt virtual mem) region which is kept at the last.
    // For virt addr mapping this master region is a dont care and hence will be init to zeroes
    uint32_t total_node_alloc_size = sizeof(qurt_elite_memorymap_node_t) + (sizeof(qurt_elite_memorymap_region_record_t) * (num_shm_reg + 1));

    // Allocate node
    qurt_elite_memorymap_node_t* mem_map_node_ptr;
    if ((NULL == (mem_map_node_ptr =(qurt_elite_memorymap_node_t *) qurt_elite_memory_aligned_malloc(
        total_node_alloc_size,8,
        QURT_ELITE_HEAP_DEFAULT)) ))
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap fail to allocate memory for the memory map node");
        return ADSP_ENOMEMORY;
    }else {
        // Initialize the node
        memset(mem_map_node_ptr, 0, total_node_alloc_size);
        mem_map_node_ptr->unNumContPhysReg = num_shm_reg;
        mem_map_node_ptr->MemPool = mem_pool_id;
        // Bookmark the first phys region:
        cont_phys_regions_ptr = (qurt_elite_memorymap_region_record_t*) ((uint8_t*)mem_map_node_ptr + sizeof(qurt_elite_memorymap_node_t));
        // Copy over each phys addr and size to the record in the node.
        for (int i=0; i<num_shm_reg; ++i) {
            cont_phys_regions_ptr[i].shm_addr.mem_addr_64b = qurt_elite_memorymap_get_physical_addr(shm_mem_reg_ptr[i].shm_addr_lsw);
//            cont_phys_regions_ptr[i].shm_addr.mem_addr_32b.msw = shm_mem_reg_ptr[i].shm_addr_msw;
            // in this case shm_addr_lsw is the virt addr
            cont_phys_regions_ptr[i].virt_addr = shm_mem_reg_ptr[i].shm_addr_lsw;
            cont_phys_regions_ptr[i].mem_size = shm_mem_reg_ptr[i].mem_size;
#ifdef DEBUG_QURT_ELITE_MEMORYMAP
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap node, successfully mapped  mem_map_node_ptr(%#x), phy addr lsw (%#x), phy addr msw (%#x), virt addr(%#x)", (unsigned int)mem_map_node_ptr, (unsigned int)cont_phys_regions_ptr[i].shm_addr.mem_addr_32b.lsw, (unsigned int)cont_phys_regions_ptr[i].shm_addr.mem_addr_32b.msw, (unsigned int)cont_phys_regions_ptr[i].virt_addr);
#endif
        }
    }

    // next, sort and add node to list.

    // Sort (bubble) the phys regions in assending order, within the node, for faster phys->virt lookup.
    for(int i=0;i<(num_shm_reg-1);++i)  {
       for(int j=0;j<(num_shm_reg-(i+1));++j)  {
           if(cont_phys_regions_ptr[j].shm_addr.mem_addr_64b > cont_phys_regions_ptr[j+1].shm_addr.mem_addr_64b) {
               qurt_elite_memorymap_region_record_t tmp = cont_phys_regions_ptr[j];
               cont_phys_regions_ptr[j] = cont_phys_regions_ptr[j+1];
               cont_phys_regions_ptr[j+1] = tmp;
           }
       }
    }


    //return mem map handle pointer
    *shm_mem_map_handle_ptr = (uint32_t)mem_map_node_ptr;

   //set the mapping type as virtual
    mem_map_node_ptr->mapping_mode = QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING;


    //add this memory map to the client
    qurt_elite_memorymap_add_mem_map_node_to_client(client_token,mem_map_node_ptr);


    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap successfully mapped all virt regions and added to client's list (token,mmhandle,num of regions) = (0x%x,0x%x,%d)",
            (unsigned int)client_token, (unsigned int)mem_map_node_ptr,  num_shm_reg);
    #endif

    return ADSP_EOK;
}

static ADSPResult qurt_elite_memorymap_region_create(
                  qurt_elite_memorymap_region_record_t * reg_ptr, //[in/out] hold info for creating a qurt region.
                  qurt_elite_memorymap_cache_mode_t cache_attr, // [in] Cache attributes.
                  QURT_ELITE_MEMORYPOOLTYPE pool_id, // [in] Pool ID.
                  qurt_mem_pool_t*     mem_pool_ptr, // [in/out] Qurt pool object, [in] when create sub regions
                                            // from reserved master region; [out] when create other regions.
                  qurt_elite_memorymap_command create_cmd // [in] Indicate the kind of region to create.
){

    int qurt_mem_result;
    qurt_mem_region_attr_t memreg_attr;

    // Determine qurt MemPool.
    switch (create_cmd) {
    case CREATE_MASTER_REGION:
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_ANY:
        //check if pool_id is valid
        if ( QURT_ELITE_MEMORYMAP_POOL_OUT_OF_RANGE <= pool_id  )
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create poolID not in range, poolID:(%d)", pool_id);
            return ADSP_EBADPARAM;
        }

        //attach to the memory pool and create the pool object.
        if (EOK != (qurt_mem_result = qurt_mem_pool_attach((char*)qurt_elite_memorymap_pool_name[pool_id],mem_pool_ptr))) {

            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create qurt_mem_pool_attach failed, poolID:(%d)", pool_id);
            return ADSP_EFAILED;
        }
        break;
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED:
        // No need to create pool object, it's [in] parameter in this case.
        break;
    }

    //initialize the mem region attribute
    qurt_mem_region_attr_init(&memreg_attr);

    //set the cache mode
    qurt_mem_region_attr_set_cache_mode(&memreg_attr, cache_attr);

    //set the mapping type of the region based on the type of region requested:
    //Normal stand alone region: QURT_MEM_MAPPING_PHYS_CONTIGUOUS
    //Master region: master region that is just to reserve virtual mem space.
    //VirtualFixed region: sub-region that falls into a master region, thus virtual addr is fixed
    switch (create_cmd) {
    case CREATE_MASTER_REGION:
        #ifdef DEBUG_QURT_ELITE_MEMORYMAP
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create creating master region, pooID:(%d)", pool_id);
        #endif
        // Just reserve space by creating master region
        qurt_mem_region_attr_set_mapping(&memreg_attr,QURT_MEM_MAPPING_NONE);
        // Make sure both physaddr and virtaddr are set to invalid
        qurt_mem_region_attr_set_virtaddr(&memreg_attr, 0xFFFFFFFF);
        qurt_mem_region_attr_set_physaddr_64(&memreg_attr, 0xFFFFFFFFFFFFFFFF);
        break;
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED:
        // This is to map a phy addr to part of an existing master region and create a sub region.
        // The Virt addr should have been set in the node before calling this func.

        #ifdef DEBUG_QURT_ELITE_MEMORYMAP
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create creating sub region, pooID:(%d)", pool_id);
        #endif

        qurt_mem_region_attr_set_mapping(&memreg_attr,QURT_MEM_MAPPING_VIRTUAL_FIXED);
        //set physical addr of the region
        qurt_mem_region_attr_set_physaddr_64(&memreg_attr, reg_ptr->shm_addr.mem_addr_64b );
        //set virtual addr of the region
        qurt_mem_region_attr_set_virtaddr(&memreg_attr, reg_ptr->virt_addr );
        break;
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_ANY:
        // This is just to map a phy addr to any virt addr, virt addr shall not
        // have been set in the node yet. It will be the output.
        #ifdef DEBUG_QURT_ELITE_MEMORYMAP
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create creating normal region, pooID:(%d)", pool_id);
        #endif

        if (0 != (reg_ptr->shm_addr.mem_addr_32b.lsw & 0xfff) ) {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap_region_create: Phy addr is not 4k aligned. (%x) returning error", (unsigned int)reg_ptr->shm_addr.mem_addr_32b.lsw);
            return ADSP_EBADPARAM;
            }


        qurt_mem_region_attr_set_mapping(&memreg_attr,QURT_MEM_MAPPING_PHYS_CONTIGUOUS);
        //set physical addr of the region
        qurt_mem_region_attr_set_physaddr_64(&memreg_attr, reg_ptr->shm_addr.mem_addr_64b );
        break;
    }

    //create the memory region
    if (EOK != (qurt_mem_result = qurt_mem_region_create(&(reg_ptr->mem_reg),
                                                 reg_ptr->mem_size,
                                                 *mem_pool_ptr,
                                                 &memreg_attr )))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create qurt_mem_region_create fails, qurt_mem_result:(%d)", qurt_mem_result);
        return ADSP_EFAILED;
    }

    // Get the Virtual addr of the created region if not creating sub region of a master region,
    // for master region, we need get the start virtual addr as well.
    // for sub-region, the virt addr is given by the caller instead of returned from qurt.
    switch (create_cmd) {
    case CREATE_MASTER_REGION:
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_ANY:
        if ((EOK != (qurt_mem_result = qurt_mem_region_attr_get(reg_ptr->mem_reg, &memreg_attr)))
            )
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_create qurt_mem_region_attr_get fails, qurt_mem_result:(%d)", qurt_mem_result);
            qurt_mem_region_delete(reg_ptr->mem_reg);
            return ADSP_EFAILED;
        }
        qurt_mem_region_attr_get_virtaddr (&memreg_attr, (unsigned int *)&(reg_ptr->virt_addr));
        break;
    case MAP_PHYSICAL_REGION_TO_VIRTUAL_FIXED:
        break;
    }

    if (create_cmd != CREATE_MASTER_REGION) {
       MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap Create Region Success [Phy LSW, Phy MSW,Virt,Size]=[%8x,%8x,%8x,%d]\n",
               (unsigned int)reg_ptr->shm_addr.mem_addr_32b.lsw,
               (unsigned int)reg_ptr->shm_addr.mem_addr_32b.msw,
               (unsigned int)reg_ptr->virt_addr,
               (unsigned int)reg_ptr->mem_size);
    }

    return ADSP_EOK;
}


ADSPResult qurt_elite_memorymap_get_mapping_mode(    uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    qurt_elite_memorymap_mapping_mode_t* mapping_mode_ptr)
{

    ADSPResult rc;

    if (NULL == mapping_mode_ptr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap mapping_mode_ptr is null while getting the mapping mode");
        return ADSP_EBADPARAM;
    }

    if (0 == shm_mem_map_handle)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap memory map handle is null while getting the mapping mode");
        return ADSP_EBADPARAM;
    }

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap cannot find the client 0x%x", (unsigned int)client_token);
        return rc;
    }
    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;

    rc = ADSP_EFAILED;

    //enter the lock
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    while (mem_map_node_ptr) {
        if (shm_mem_map_handle == (uint32_t)mem_map_node_ptr)
        {
            *mapping_mode_ptr = (qurt_elite_memorymap_mapping_mode_t)(mem_map_node_ptr->mapping_mode);
            rc = ADSP_EOK;
            break;
        }
        mem_map_node_ptr = mem_map_node_ptr->pNext;
    }

    //leave the lock
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_get_mapping mode (mem handle, mapping mode, rc)->(0x%x,%d,0x%x)",
          (unsigned int)shm_mem_map_handle,(unsigned int)*mapping_mode_ptr, rc);
    #endif

    return rc;
}


ADSPResult qurt_elite_memorymap_shm_mem_unmap(uint32_t client_token,
                                        uint32_t shm_mem_map_handle
                                    )
{
    ADSPResult rc;

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {

        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap cannot find the client 0x%x", (unsigned int)client_token);
        return rc;
    }

    //check if mmhandle is NULL
    if (0== shm_mem_map_handle)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap null memory map handle received for the client 0x%x", (unsigned int)client_token);
    }

    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;
    qurt_elite_memorymap_node_t *prev_ptr = NULL;
    qurt_elite_memorymap_node_t *found_mem_map_node_ptr = NULL;

    //lock the access of the list
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    //search and compare with the first physical region record in all the memory mapping nodes of the client.
    while (mem_map_node_ptr) {
        if  (shm_mem_map_handle ==  (uint32_t)mem_map_node_ptr)
        {
            found_mem_map_node_ptr = (qurt_elite_memorymap_node_t *)shm_mem_map_handle;
            break;
        }
        //store current as prev and move current to next
        prev_ptr = mem_map_node_ptr;
        mem_map_node_ptr = mem_map_node_ptr->pNext;
    }

    //delete the qube memory region and regardless of the result of qurt_elite_memorymap_region_delete()
    // The node must be deleted from the list
    if (found_mem_map_node_ptr)
    {
        if (0 != found_mem_map_node_ptr->ref_count )
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap non zero ref count detected, cannot unmap the node  (client token,mmhandle,ref count)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)found_mem_map_node_ptr,found_mem_map_node_ptr->ref_count);
			rc = ADSP_ENOTREADY;
			goto shm_mem_unmap_bail_out;

        }


        if (ADSP_EOK != (rc = qurt_elite_memorymap_region_delete(found_mem_map_node_ptr)))
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_delete failed (client token,mmhandle,status)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)found_mem_map_node_ptr,rc);
        }

        //delete the node
        // if it is the first node pointed by the pMemMapListNode, then point pMemMapListNode to the mem_map_node_ptr->pNext
        if (NULL == prev_ptr)
        {
            c_ptr->pMemMapListNode = found_mem_map_node_ptr->pNext;
        }
        else
        {
            // not first, keep previous connecting to the next
            prev_ptr->pNext = found_mem_map_node_ptr->pNext;
        }

        //update the Global state
        qurt_elite_atomic_subtract(&(qurt_elite_globalstate.nMemRegions), found_mem_map_node_ptr->unNumContPhysReg);

		//free up the resources
        qurt_elite_memory_aligned_free(found_mem_map_node_ptr);
    }
    else
    {
        //Could not find it and hence return unsupported.

        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap meory map handle not found (client token,mmhandle,status)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)shm_mem_map_handle,rc);


        //TBD is it ADSP_EBADPARAM or ADSP_EUNSUPPORTED ??
        rc = ADSP_EBADPARAM;
    }
    shm_mem_unmap_bail_out:
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

 #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap memory unmap (client token,mmhandle,status)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)shm_mem_map_handle,rc);
#endif
    return rc;

}

static ADSPResult qurt_elite_memorymap_region_delete(qurt_elite_memorymap_node_t *mem_map_node_ptr)
{

    // TBD client must call flush explicity before calling unmap physical memory.
    //here just invalidate cache, since it is not possible to know if flush is required or Not.

    int qurt_mem_result;
    if (QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING == mem_map_node_ptr->mapping_mode) {
        // Bookmark the first phys region:
        qurt_elite_memorymap_region_record_t* cont_phys_regions_ptr = (qurt_elite_memorymap_region_record_t*) ((uint8_t*)mem_map_node_ptr + sizeof(qurt_elite_memorymap_node_t));
        for (int i=0; i<mem_map_node_ptr->unNumContPhysReg; ++i) {
            if( (EOK != (qurt_mem_result = qurt_mem_cache_clean( (qurt_addr_t)(cont_phys_regions_ptr[i].virt_addr),
                                                                cont_phys_regions_ptr[i].mem_size,
                                                                QURT_MEM_CACHE_INVALIDATE,
                                                                QURT_MEM_DCACHE )))
               || (EOK != ( qurt_mem_result = qurt_mem_region_delete(cont_phys_regions_ptr[i].mem_reg))) )
            {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_delete qurt_mem_region_delete fails, qurt_mem_result:(%d)", qurt_mem_result);
                return ADSP_EFAILED;
            }
        }
        // At last, delete the master region, if exist (exists when the number of phys region is more than 1).
        if (0 < mem_map_node_ptr->unNumContPhysReg) {
            // Delete master region, DO NOT use qurt_elite_memorymap_region_delete() since no need to invalidate cache.
            // directly delete it from qurt.
            qurt_mem_region_delete(cont_phys_regions_ptr[mem_map_node_ptr->unNumContPhysReg].mem_reg);
        }
    }

    return ADSP_EOK;

}


ADSPResult qurt_elite_memorymap_unmap_all(uint32_t client_token)
{
    ADSPResult rc;

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {

        return rc;
    }
    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;

    qurt_elite_memorymap_node_t *current_mem_map_node_ptr = NULL;


    //lock the access of the list
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    //unmap all regions of the client one at a time.
    while (mem_map_node_ptr) {
        current_mem_map_node_ptr = mem_map_node_ptr;

        if (0 != current_mem_map_node_ptr->ref_count )
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap non zero ref count detected, cannot unmap the node  (client token,mmhandle,ref count)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)current_mem_map_node_ptr,current_mem_map_node_ptr->ref_count);
            rc = ADSP_ENOTREADY;
			goto shm_mem_unmap_all_bail_out;

        }


        if (ADSP_EOK != (rc = qurt_elite_memorymap_region_delete(current_mem_map_node_ptr)))
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_region_delete failed (client token,mem map node, status)->(0x%x,0x%x,0x%x)",
                    (unsigned int)client_token,(unsigned int)current_mem_map_node_ptr,rc);
        }
        //update the Global state
        qurt_elite_atomic_subtract(&(qurt_elite_globalstate.nMemRegions), mem_map_node_ptr->unNumContPhysReg);
        mem_map_node_ptr = mem_map_node_ptr->pNext;
        //free up the resources
        qurt_elite_memory_aligned_free(current_mem_map_node_ptr);
    }
    c_ptr->pMemMapListNode = NULL;
    //Unlock the access of the list

shm_mem_unmap_all_bail_out:
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

#ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap memory unmap all (client token,status)->(0x%x,0x%x)"
            , (unsigned int)client_token,rc);
#endif


    return ADSP_EOK;
}


ADSPResult qurt_elite_memorymap_unregister(uint32_t client_token)
{

    ADSPResult rc = ADSP_EOK;
    //unmap all regions in the client
    if (ADSP_EOK != (rc = qurt_elite_memorymap_unmap_all(client_token) )) {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap failed to unmap all regions of client (token,0x%x)", (unsigned int)client_token);
    }
    //Still proceed.. if the above step fails, it introduces memory leaks.
    qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);

    qurt_elite_memorymap_client_t *delete_client_ptr = (qurt_elite_memorymap_client_t *)client_token;
    //destroy the client mutex before freeing the client.
    qurt_elite_mutex_destroy(&(delete_client_ptr->mClientMutex));
    qurt_elite_memorymap_client_t *prev_ptr = qurt_elite_globalstate.pMemoryMapClientList;
    qurt_elite_memorymap_client_t *current_ptr = prev_ptr;

    //check if the current client is same as qurt_elite_globalstate.pMemoryMapClientList

    if (delete_client_ptr == qurt_elite_globalstate.pMemoryMapClientList ) {
        //delete the node
        qurt_elite_globalstate.pMemoryMapClientList = delete_client_ptr->pNext;
        //free up the resource
        qurt_elite_memory_free(delete_client_ptr);
    }
    else
    {
        while (current_ptr) {

            if (current_ptr == delete_client_ptr ) {

                prev_ptr->pNext = delete_client_ptr->pNext;
                //free up the resource
                qurt_elite_memory_free(delete_client_ptr);
                break;

            }
            prev_ptr = current_ptr;
            current_ptr = current_ptr->pNext;
        }
    }
    //unlock the global mutex
    qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap unregister client (client token,status)->(0x%x,0x%x)"
            , (unsigned int)client_token,rc);

    return rc;
}

//not thread safe.  The caller of this function has to ensure thread safety.
static uint32_t qurt_elite_memorymap_search_virtual_addr_in_node(
    qurt_elite_memorymap_node_t *mem_map_node_ptr,
    uint64_t phy_addr_64bits_inp,
    qurt_elite_memorymap_mem_region_attrib_t *mem_reg_attrib_ptr,
    bool_t get_reg_attrib_flag
)
{
  uint64_t phy_addr_64bits = phy_addr_64bits_inp;
  uint32_t req_virt_addr =0;
  qurt_elite_memorymap_mem_region_attrib_t local_mem_reg_attrib;

  //initialze the memory region attribute to zero
  memset(&local_mem_reg_attrib, 0, sizeof(qurt_elite_memorymap_mem_region_attrib_t));

  // Bookmark the first phys region:
  qurt_elite_memorymap_region_record_t* cont_phy_regions_ptr =
      (qurt_elite_memorymap_region_record_t*) ((uint8_t*)mem_map_node_ptr + sizeof(qurt_elite_memorymap_node_t));

  //for virtual mapping mode, phy_addr_64bits_inp is interpreted as virtual address. Hence convert
  //from virtual to physical and then do the search.

  if (QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING == mem_map_node_ptr->mapping_mode)
  {
    phy_addr_64bits = qurt_elite_memorymap_get_physical_addr(uint32_t(phy_addr_64bits_inp & 0xffffffff));
  }

  //At this stage phy_addr_64bits is the actual physcial addr for which the virt address and the memory region containing that virt addr must be searched.
  //The search must be performed by default.

  int16_t left = 0;
  int16_t right = mem_map_node_ptr->unNumContPhysReg - 1;
  int16_t middle = 0;
  bool_t found = FALSE;

  while(left <= right) {
    middle = (left + right) >> 1; // Although right could be negative -1, but left+right must not be negative,
    // so it's safe to shift as using divid-by-2.

    if((phy_addr_64bits >= cont_phy_regions_ptr[middle].shm_addr.mem_addr_64b) &&
        (phy_addr_64bits <= cont_phy_regions_ptr[middle].shm_addr.mem_addr_64b + (uint64_t)cont_phy_regions_ptr[middle].mem_size - 1) ) {
      found = TRUE;
      break;
    } else {
      if(phy_addr_64bits < cont_phy_regions_ptr[middle].shm_addr.mem_addr_64b) right = middle - 1;
      if(phy_addr_64bits > cont_phy_regions_ptr[middle].shm_addr.mem_addr_64b + (uint64_t)cont_phy_regions_ptr[middle].mem_size - 1) left = middle + 1;
    }
  }

#ifdef DEBUG_QURT_ELITE_MEMORYMAP
  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap get virt_addr (isFound??,phy_addr_msw,phy_addr_lsw)->(0x%x,0x%x,0x%x)",
        found,(unsigned int)((uint64_t)phy_addr_64bits>>32),(unsigned int)phy_addr_64bits);
#endif
  if (found) {
    req_virt_addr =  (cont_phy_regions_ptr[middle].virt_addr + (uint32_t)(phy_addr_64bits - cont_phy_regions_ptr[middle].shm_addr.mem_addr_64b));
    //store the memory region index that found the corresponding virt addr
    local_mem_reg_attrib.mem_reg_base_phy_addr_lsw = cont_phy_regions_ptr[middle].shm_addr.mem_addr_32b.lsw;
    local_mem_reg_attrib.mem_reg_base_phy_addr_msw = cont_phy_regions_ptr[middle].shm_addr.mem_addr_32b.msw;
    local_mem_reg_attrib.mem_reg_base_virt_addr = cont_phy_regions_ptr[middle].virt_addr;
    local_mem_reg_attrib.mem_reg_size = cont_phy_regions_ptr[middle].mem_size;
    local_mem_reg_attrib.req_virt_adrr = req_virt_addr;
    //return the remaining size as the size inclusive of the requested virtual address
    local_mem_reg_attrib.rem_reg_size =  cont_phy_regions_ptr[middle].virt_addr+ cont_phy_regions_ptr[middle].mem_size - req_virt_addr;

  }else {
    //no need to explicitly set local_mem_reg_attrib, since it is already initialized to zero in the beginning of this function
    req_virt_addr = 0;

  }

  //check if attribute needs to be filled before returning
  if (TRUE == get_reg_attrib_flag)
  {
    memscpy((void *)mem_reg_attrib_ptr, sizeof(qurt_elite_memorymap_mem_region_attrib_t), (void *)&local_mem_reg_attrib, sizeof(qurt_elite_memorymap_mem_region_attrib_t));
  }

  return req_virt_addr;

}

ADSPResult qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    uint32_t *virt_addr_ptr)
{

    ADSPResult rc;

    if (0== shm_mem_map_handle )
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap memory map handle is null");
        return ADSP_EFAILED;
    }

    if (NULL == virt_addr_ptr  )
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap virt_addr_ptr is null");
        return ADSP_EFAILED;
    }

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {

        return rc;
    }
    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;
    //return zero vaddr if phy addr not found.
    *virt_addr_ptr = 0;
    rc = ADSP_EFAILED;
    //
    uint64_t phy_addr_64bits = ((uint64_t)shm_addr_msw << 32)|(shm_addr_lsw);

    qurt_elite_memorymap_node_t *tmp_mem_map_node_ptr = (qurt_elite_memorymap_node_t *)shm_mem_map_handle;
    //enter the lock
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    while (mem_map_node_ptr) {
        if (shm_mem_map_handle == (uint32_t)mem_map_node_ptr)
        {
            if (NULL != (*virt_addr_ptr = qurt_elite_memorymap_search_virtual_addr_in_node(mem_map_node_ptr, phy_addr_64bits,NULL,FALSE)))
            {
                rc = ADSP_EOK;
            }
            break;
        }
        mem_map_node_ptr = mem_map_node_ptr->pNext;
    }

    //check if corresponding phy addr is available for that virt addr
    if ((ADSP_EOK) == rc )
    {
        uint64_t phy_addr_64bits_fr_virt = qurt_elite_memorymap_get_physical_addr(*virt_addr_ptr);
        if ((0 == phy_addr_64bits_fr_virt)||
            ((QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING == tmp_mem_map_node_ptr->mapping_mode)&& (phy_addr_64bits != phy_addr_64bits_fr_virt))
            )
        {
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "qurt_elite_memorymap get_virtual_addr (paddr msw,paddr lsw, vaddr)->(0x%x,0x%x,0x%x) failed since qurt and qurt elite mapping is out of sync",
            (unsigned int)shm_addr_msw,(unsigned int)shm_addr_lsw,(unsigned int)*virt_addr_ptr);
            rc = ADSP_EFAILED;
        }

    }

    //leave the lock
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap qurt_elite_memorymap_get_virtual_addr (paddr msw,paddr lsw, vaddr)->(0x%x,0x%x,0x%x)",
            (unsigned int)shm_addr_msw,(unsigned int)shm_addr_lsw,(unsigned int)*virt_addr_ptr);
    #endif

    return rc;
}


ADSPResult qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    qurt_elite_memorymap_mem_region_attrib_t *mem_reg_attrib_ptr)
{

    ADSPResult rc;
    uint32_t virt_addr=0;

    if (0== shm_mem_map_handle )
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap memory map handle is null");
        return ADSP_EFAILED;
    }

    if (NULL == mem_reg_attrib_ptr  )
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap mem_reg_attrib_ptr is null");
        return ADSP_EFAILED;
    }

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {

        return rc;
    }
    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;
    rc = ADSP_EFAILED;
    //
    uint64_t phy_addr_64bits = ((uint64_t)shm_addr_msw << 32)|(shm_addr_lsw);

    qurt_elite_memorymap_node_t *tmp_mem_map_node_ptr = (qurt_elite_memorymap_node_t *)shm_mem_map_handle;
    //enter the lock
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    while (mem_map_node_ptr) {
        if (shm_mem_map_handle == (uint32_t)mem_map_node_ptr)
        {
            if (NULL != (virt_addr = qurt_elite_memorymap_search_virtual_addr_in_node(mem_map_node_ptr, phy_addr_64bits,mem_reg_attrib_ptr,TRUE)))
            {
                rc = ADSP_EOK;
            }
            break;
        }
        mem_map_node_ptr = mem_map_node_ptr->pNext;
    }

    //check if corresponding phy addr is available for that virt addr
    if ((ADSP_EOK) == rc )
    {
        uint64_t phy_addr_64bits_fr_virt = qurt_elite_memorymap_get_physical_addr(virt_addr);
        if ((0 == phy_addr_64bits_fr_virt)||
            ((QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING == tmp_mem_map_node_ptr->mapping_mode)&& (phy_addr_64bits != phy_addr_64bits_fr_virt))
            )
        {
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle (paddr msw,paddr lsw, vaddr)->(0x%x,0x%x,0x%x) failed since qurt and qurt elite mapping is out of sync",
            (unsigned int)shm_addr_msw,(unsigned int)shm_addr_lsw,(unsigned int)virt_addr);

        }

    }

    //leave the lock
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle (paddr msw,paddr lsw, vaddr)->(0x%x,0x%x,0x%x)",
            (unsigned int)shm_addr_msw,(unsigned int)shm_addr_lsw,(unsigned int)virt_addr);
    #endif

    return rc;
}

static ADSPResult qurt_elite_memorymap_client_find(uint32_t client_token)
{
    ADSPResult rc = ADSP_EHANDLE;
    qurt_elite_memorymap_client_t *found_client_ptr = (qurt_elite_memorymap_client_t *)client_token;

    QURT_ELITE_ASSERT(client_token);

    //enter the lock
    qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
    qurt_elite_memorymap_client_t *current_ptr = qurt_elite_globalstate.pMemoryMapClientList;
    while (current_ptr) {
        if (current_ptr == found_client_ptr) {
            rc = ADSP_EOK;
            break;
        }
        current_ptr = current_ptr->pNext;
    }
    //leave the lock
    qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);
    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"qurt_elite_memorymap, client found (client token,status)->(0x%x,0x%x)",(unsigned int)client_token,rc);
    #endif
    return rc;
}


static ADSPResult qurt_elite_memorymap_verify_pool_range(uint32_t shm_addr,uint32_t mem_size, QURT_ELITE_MEMORYPOOLTYPE *pool_id_ptr)
{
    ADSPResult rc = ADSP_EOK;

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap_verify_pool_range Begin: shm_addr:(0x%x), inputPool:(%d)", (unsigned int)shm_addr, (unsigned int)*pool_id_ptr);
    #endif
#if (!defined(SIM))
   #ifndef MDSPMODE
    uint32_t ebi_start = (uint32_t) EBI_START;
   #endif
    switch (*pool_id_ptr) {

    case QURT_ELITE_MEMORYMAP_DEFAULTPHY_POOL:
    case QURT_ELITE_MEMORYMAP_EBI1_POOL:
    #ifndef MDSPMODE
        // check is phyaddr is in in EBI1 pool
        if ( (shm_addr < ebi_start)
             || ((shm_addr + mem_size - 1) > (((uint32_t)(EBI_START + EBI_SIZE)))))
        {
            rc = ADSP_EFAILED;
        }
        break;
   #endif
    case QURT_ELITE_MEMORYMAP_SMI_POOL:
 #ifndef MDSPMODE
        if  (((shm_addr) < ((uint32_t) SMI_POOL_BASE))
             || ((shm_addr + mem_size - 1) > (((uint32_t) (SMI_POOL_BASE + SMI_POOL_SIZE)))))
        {
            rc = ADSP_EFAILED;
        }
        break;
#endif
    case QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL:
     #ifndef MDSPMODE
        //Search with in shared memory pools, which is EBI and SMI at this point.
        if (((shm_addr) >= (ebi_start))
             && ((shm_addr + mem_size - 1) <= (((uint32_t)(EBI_START + EBI_SIZE)))))
        {
            *pool_id_ptr = QURT_ELITE_MEMORYMAP_EBI1_POOL;
        }
        else if (((shm_addr) >= ((uint32_t) SMI_POOL_BASE))
             && ((shm_addr + mem_size - 1) <= (((uint32_t) (SMI_POOL_BASE + SMI_POOL_SIZE)))))
        {
            *pool_id_ptr = QURT_ELITE_MEMORYMAP_SMI_POOL;
        }
        else
        {
            rc = ADSP_EFAILED;
        }
  #else
     *pool_id_ptr = QURT_ELITE_MEMORYMAP_EBI1_POOL;
#endif
        break;

    default:
        rc = ADSP_EFAILED;
    }
#endif

    #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap_verify_pool_range End: shm_addr:(0x%x), outputPool:(%d)", (unsigned int)shm_addr, (unsigned int)*pool_id_ptr);
    #endif
    return rc;
}


ADSPResult qurt_elite_memorymap_shm_incr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    )
{
    return qurt_elite_memorymap_shm_update_refcount(client_token,shm_mem_map_handle,1);
}


ADSPResult qurt_elite_memorymap_shm_decr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    )
{
    return qurt_elite_memorymap_shm_update_refcount(client_token,shm_mem_map_handle,-1);
}


static ADSPResult qurt_elite_memorymap_shm_update_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    int16_t count_value
                    )
{
    ADSPResult rc;

    // no lock to access the Client, since the assumption is ideally client register once and does not unregister.
    // Even if it unregister, Client must call unregister after ensuring all its dynamic services have exit.
    if (ADSP_EOK != (rc = qurt_elite_memorymap_client_find(client_token))) {

        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap cannot find the client 0x%x", (unsigned int)client_token);
        return rc;
    }

    //check if mmhandle is NULL
    if (0 == shm_mem_map_handle)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap null memory map handle received for the client 0x%x", (unsigned int)client_token);
    }

    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;

    qurt_elite_memorymap_node_t *found_mem_map_node_ptr = NULL;

    //lock the access of the list
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    qurt_elite_memorymap_node_t *mem_map_node_ptr = c_ptr->pMemMapListNode;
    //search and compare with the first physical region record in all the memory mapping nodes of the client.
    while (mem_map_node_ptr) {
        if  (shm_mem_map_handle ==  (uint32_t)mem_map_node_ptr)
        {
            found_mem_map_node_ptr = (qurt_elite_memorymap_node_t *)shm_mem_map_handle;
            break;
        }
        //store current as prev and move current to next

        mem_map_node_ptr = mem_map_node_ptr->pNext;
    }

    //delete the qube memory region and regardless of the result of qurt_elite_memorymap_region_delete()
    // The node must be deleted from the list
    if (found_mem_map_node_ptr)
    {
        found_mem_map_node_ptr->ref_count+= count_value;

    }
    else
    {
        //Could not find it and hence return unsupported.

        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap meory map handle not found (client token,mmhandle,status)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)shm_mem_map_handle,rc);


        //TBD is it ADSP_EBADPARAM or ADSP_EUNSUPPORTED ??
        rc = ADSP_EBADPARAM;
    }

    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));

 #ifdef DEBUG_QURT_ELITE_MEMORYMAP
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap update ref count (client token,mmhandle,status)->(0x%x,0x%x,0x%x)"
            , (unsigned int)client_token,(unsigned int)shm_mem_map_handle,rc);
#endif
    return rc;


}


static void qurt_elite_memorymap_add_mem_map_node_to_client(uint32_t client_token,
                                                                  qurt_elite_memorymap_node_t *mem_map_node_ptr
                                                                  )
{
    qurt_elite_memorymap_client_t *c_ptr = (qurt_elite_memorymap_client_t *)client_token;
    //add this list of regions to the client
    qurt_elite_mutex_lock(&(c_ptr->mClientMutex));
    mem_map_node_ptr->pNext = c_ptr->pMemMapListNode;
    c_ptr->pMemMapListNode = mem_map_node_ptr;
    qurt_elite_mutex_unlock(&(c_ptr->mClientMutex));
    // increment the counter of memRegions.
    qurt_elite_atomic_add(&(qurt_elite_globalstate.nMemRegions), (uint32_t)mem_map_node_ptr->unNumContPhysReg);

}

/**
  Flushes the cache (data) of the specified memory region.

  @param[in] virt_addr  Starting virtual address.
  @param[in] mem_size   Size of the region to be flushed.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
ADSPResult qurt_elite_memorymap_cache_flush(uint32_t virt_addr,
                      uint32_t mem_size)
{

#if (!defined(SIM))||(__QDSP6_ARCH__ >= 3)

    int qurt_mem_result;

    if( (EOK != (qurt_mem_result = qurt_mem_cache_clean( (qurt_addr_t)(virt_addr),
                                mem_size,
                                QURT_MEM_CACHE_FLUSH,
                                QURT_MEM_DCACHE ))))
    {
        return ADSP_EFAILED;
    }


#endif //__QDSP6_ARCH__

    return ADSP_EOK;
}


ADSPResult qurt_elite_memorymap_cache_invalidate(uint32_t virt_addr,
                       uint32_t mem_size)
{

#if (!defined(SIM))||(__QDSP6_ARCH__ >= 3)

    int qurt_mem_result;

    if( (EOK != (qurt_mem_result = qurt_mem_cache_clean( (qurt_addr_t)(virt_addr),
                                mem_size,
                                QURT_MEM_CACHE_INVALIDATE,
                                QURT_MEM_DCACHE ))))

    {
        return ADSP_EFAILED;
    }

#endif //__QDSP6_ARCH__

    return ADSP_EOK;
}

/**
  Given a virtual address, retrieves the corresponding mapping entry from
  the page table, adjusts the offset based on the page size, and returns the
  full 64-bit physical address back to the user.

  @param[in] virt_addr    Starting virtual address.

  @return
  Physical address if the address is mapped; otherwise, 0.

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
uint64_t qurt_elite_memorymap_get_physical_addr(uint32_t virt_addr){

    uint64_t phy_addr = (uint64_t)(qurt_lookup_physaddr_64 (virt_addr));
    return phy_addr;

}

ADSPResult qurt_elite_memorymap_add_remove_pool_page(bool_t is_add,
      QURT_ELITE_MEMORYPOOLTYPE pool_id, uint32_t size, uint64_t phy_addr)
{
   ADSPResult result = ADSP_EOK;
   qurt_mem_pool_t pool;
   //size and start addr must be a multiple of 4K, otherwise return an error
   if ((size & 0xFFF) || (phy_addr & 0xFFF))
   {
      result = ADSP_EBADPARAM;
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap_add_remove_pool_page size or start addr is not a multiple of 4K (addr, size, status)->(0x%x, 0x%x, 0x%x)",
            phy_addr, size, result);
      return result;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap_add_remove_pool_page  poolID:(%d, 0x%lx%lx, %lx)", pool_id, (uint32_t)(phy_addr>>32), (uint32_t)phy_addr, size);

   int r = qurt_mem_pool_attach((char*)qurt_elite_memorymap_pool_name[pool_id], &pool);
   if (EOK != r)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_memorymap_add_remove_pool_page  qurt_mem_pool_attach failed, poolID:(%d)", pool_id);
      return ADSP_EFAILED;
   }

   int handle = qurt_elite_qdi_open();
   result = qurt_elite_qdi_add_remove_pages(handle, pool, is_add, (uint32_t)(phy_addr >> 12), size >> 12);
   qurt_elite_qdi_close(handle);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_memorymap_add_remove_pool_page done. poolID:(%d, 0x%lx%lx, %lx). result 0x%lx",
         pool_id, (uint32_t)(phy_addr>>32), (uint32_t)phy_addr, size, result);

   return result;
}
