/*========================================================================

This file implements utility functions to manage shared memory between
scorpion and Qdsp6, including physical addresses to virtual address mapping, etc.

Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/EliteMem_Util.cpp#1 $$DateTime: 2017/11/07 23:00:25 $$Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/12/09   DC      Created file.
03/28/11   AZ      Support virtual contiguous memory.

========================================================================== */
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
// system
#include "qurt_elite.h"

// Audio
#include "EliteMem_Util.h"

#include "qurt_elite_memorymap.h"
#include "adsp_audio_memmap_api.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define ELITEMEM_UTIL_DEBUG
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

ADSPResult elite_mem_map_get_shm_attrib(uint32_t unPhysAddrLsw,
                                uint32_t unPhysAddrMsw,
                                uint32_t unMemSize,
                                elite_mem_shared_memory_map_t *pShareMemMapTypeNode ) {

    ADSPResult nRet = ADSP_EOK;

    uint32_t unVirtAddrEnd,unVirtAddrStart;

    QURT_ELITE_ASSERT(pShareMemMapTypeNode);
    //even though memsize is uinsigned, memsize >= than 0x80000000(2Gig) is invalid for aDSP and
    //memsize =0 is also invalid. The below check will take care of this.
    if ((int32_t)unMemSize <= 0) {

        return ADSP_EBADPARAM;
    }

    if (ADSP_FAILED(nRet = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(pShareMemMapTypeNode->unMemMapClient,
                                                                                    pShareMemMapTypeNode->unMemMapHandle,
                                                                                    unPhysAddrLsw,
                                                                                    unPhysAddrMsw,
                                                                                    &unVirtAddrStart)))
    {
       MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib failed, & input is [mapclient, maphandle, PhyMsw,PhyLsw]=[%8x,%8x,%8x,%8x],error %#x\n",
               (unsigned int)pShareMemMapTypeNode->unMemMapClient,
               (unsigned int)pShareMemMapTypeNode->unMemMapHandle,
               (unsigned int)unPhysAddrMsw,
               (unsigned int)unPhysAddrLsw,
             nRet
             );
       MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib failed & sharednode is [PhyMsw,PhyLsw,Virt,Size]=[%8x,%8x,%8x,%8x],error %#x\n",
               (unsigned int)pShareMemMapTypeNode->unPhysAddrMsw,
               (unsigned int)pShareMemMapTypeNode->unPhysAddrLsw,
               (unsigned int)pShareMemMapTypeNode->unVirtAddr,
               (unsigned int)pShareMemMapTypeNode->unMemSize,
             nRet
             );
        return nRet;
    }

    unVirtAddrEnd = unVirtAddrStart + unMemSize-1;
    uint64_t unPhyAddrEnd = qurt_elite_memorymap_get_physical_addr(unVirtAddrEnd);

    if (0 == unPhyAddrEnd)
    {

        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib failed, & input is [mapclient, maphandle, PhyMsw,PhyLsw]=[%8x,%8x,%8x,%8x] \n",
               (unsigned int)pShareMemMapTypeNode->unMemMapClient,
               (unsigned int)pShareMemMapTypeNode->unMemMapHandle,
               (unsigned int)unPhysAddrMsw,
                (unsigned int)unPhysAddrLsw
             );

        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib failed to get End Physical Addr, [virtAddrEnd, VirtAddrStart]=[%8x,%8x]\n",
                (unsigned int)unVirtAddrEnd,
                (unsigned int)unVirtAddrStart
             );

        return ADSP_EFAILED;
    }
#ifdef ELITEMEM_UTIL_DEBUG
    else
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib End Physical Addr, [unPhyAddrEnd LSW, MSW]=[%8x,%8x]\n",
                (unsigned int)unPhyAddrEnd,
                (unsigned int)(unPhyAddrEnd>>32)
              );

    }
#endif

    // Fill other field of the memory map struct.
    pShareMemMapTypeNode->unMemSize  = unMemSize;
    pShareMemMapTypeNode->unPhysAddrLsw = unPhysAddrLsw;
    pShareMemMapTypeNode->unPhysAddrMsw = unPhysAddrMsw;
    pShareMemMapTypeNode->unVirtAddr = unVirtAddrStart;

#ifdef ELITEMEM_UTIL_DEBUG
    MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "[PhyMsw,PhyLsw,Virt,Size]=[%8x,%8x,%8x,%d]\n",
            (unsigned int)pShareMemMapTypeNode->unPhysAddrMsw,
            (unsigned int)pShareMemMapTypeNode->unPhysAddrLsw,
            (unsigned int)pShareMemMapTypeNode->unVirtAddr,
            (unsigned int)pShareMemMapTypeNode->unMemSize);
#endif
    return ADSP_EOK;
}


ADSPResult elite_mem_map_get_shm_attrib_ref_counted(uint32_t unPhysAddrLsw,
                                    uint32_t unPhysAddrMsw,
                                    uint32_t unMemSize,
                                    elite_mem_shared_memory_map_t *pShareMemMapTypeNode )
{

    ADSPResult rc = ADSP_EOK;

    if (ADSP_FAILED(rc= elite_mem_map_get_shm_attrib(unPhysAddrLsw,unPhysAddrMsw,unMemSize,pShareMemMapTypeNode) ))
    {
        return rc;
    }

    //At this stage pShareMemMapTypeNode should have correct values

    //Incr the ref count

    if (ADSP_FAILED(rc= qurt_elite_memorymap_shm_incr_refcount(pShareMemMapTypeNode->unMemMapClient,
            pShareMemMapTypeNode->unMemMapHandle)))
    {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_get_shm_attrib_ref_counted failed when adding ref count for [mapclient, maphandle]=[%8x,%8x],error %#x\n",
                   (unsigned int)pShareMemMapTypeNode->unMemMapClient,
                   (unsigned int)pShareMemMapTypeNode->unMemMapHandle,
                   rc
                   );

    }

    return rc;

}

ADSPResult elite_mem_map_release_shm(elite_mem_shared_memory_map_t *pShareMemMapTypeNode )
{

    ADSPResult rc = ADSP_EOK;

    if (!pShareMemMapTypeNode)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "pShareMemMapTypeNode revd is NULL");
        return ADSP_EFAILED;
    }

    if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_decr_refcount(pShareMemMapTypeNode->unMemMapClient,
                                            pShareMemMapTypeNode->unMemMapHandle
                                        )))
    {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_mem_map_release_shm failed when decr ref count for [mapclient, maphandle]=[%8x,%8x],error %#x\n",
                   (unsigned int)pShareMemMapTypeNode->unMemMapClient,
                   (unsigned int)pShareMemMapTypeNode->unMemMapHandle,
                   rc
                   );

    }

    return rc;
}



ADSPResult elite_mem_flush_cache( elite_mem_shared_memory_map_t *pSharedMemMapTypeNode )
{
    ADSPResult rc = ADSP_EOK;
    if (pSharedMemMapTypeNode->unVirtAddr) {
        if (ADSP_FAILED(rc = qurt_elite_memorymap_cache_flush(pSharedMemMapTypeNode->unVirtAddr,pSharedMemMapTypeNode->unMemSize)))
        {
            return rc;
        }
    }
    return rc;
}

ADSPResult elite_mem_invalidate_cache( elite_mem_shared_memory_map_t *pSharedMemMapTypeNode )
{
    ADSPResult rc = ADSP_EOK;

    if (pSharedMemMapTypeNode->unVirtAddr) {
        if(ADSP_FAILED(rc=qurt_elite_memorymap_cache_invalidate(pSharedMemMapTypeNode->unVirtAddr,pSharedMemMapTypeNode->unMemSize)))
        {
            return rc;
        }
    }
    return rc;
}

ADSPResult elite_mem_shared_memory_map_regions_cmd_handler(uint32_t nMemMapClient,
                                      elite_apr_handle_t hAprHandle,
                                      elite_apr_packet_t *pPkt,
                                      uint32_t nRespOpcode)
{
    ADSPResult rc = ADSP_EOK;
    void * pPayload,*pDummy;
    avs_cmd_shared_mem_map_regions_t *mem_map_regions_payload_ptr;
    uint32_t mem_map_handle=0;
    elite_apr_packet_t *pRespPkt = NULL;
    QURT_ELITE_MEMORYPOOLTYPE eMemPool;
    avs_shared_map_region_payload_t **ppRegion;

    elite_apr_if_get_payload( (void**) &pPayload, pPkt );

    mem_map_regions_payload_ptr = (avs_cmd_shared_mem_map_regions_t *)pPayload;


    switch (mem_map_regions_payload_ptr->mem_pool_id)
    {
        case ADSP_MEMORY_MAP_SHMEM8_4K_POOL:
            eMemPool = QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL;
            break;
        default:

           rc = ADSP_EBADPARAM;
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem MemMapRegCmdHandler: Received Invalid PoolID: %d", mem_map_regions_payload_ptr->mem_pool_id);
           goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_1;
    }

     /* Allocate the response packet */
     rc = elite_apr_if_alloc_cmd_rsp(
         hAprHandle,
         elite_apr_if_get_dst_addr( pPkt),
         elite_apr_if_get_dst_port( pPkt),
         elite_apr_if_get_src_addr( pPkt),
         elite_apr_if_get_src_port( pPkt),
         elite_apr_if_get_client_token( pPkt),
         nRespOpcode,
         sizeof(avs_cmdrsp_shared_mem_map_regions_t),
         &pRespPkt );

    if (ADSP_FAILED(rc) || NULL == pRespPkt)
    {
        rc = ADSP_ENOMEMORY;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem MemMapRegCmdHandler: memory map response packet allocation failed with error code  = %d!", rc);
        goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_1;
    }

    /* prepare the Response payload pointer */
    avs_cmdrsp_shared_mem_map_regions_t *pRespPayload;
    elite_apr_if_get_payload((&pDummy), pRespPkt);
    pRespPayload = (avs_cmdrsp_shared_mem_map_regions_t *)pDummy;


    pDummy = ((uint8_t *)pPayload + sizeof(avs_cmd_shared_mem_map_regions_t));
    ppRegion = (avs_shared_map_region_payload_t **)&pDummy;

    // allocate qurt_elite_memorymap_shm_region_t, why not direct cast? because want to be free from
    // api data structure change or any compiler pack
    qurt_elite_memorymap_shm_region_t* phy_regions;
    if (NULL == (phy_regions = (qurt_elite_memorymap_shm_region_t*) qurt_elite_memory_malloc(
        sizeof(qurt_elite_memorymap_shm_region_t) * mem_map_regions_payload_ptr->num_regions,
        QURT_ELITE_HEAP_DEFAULT)))
    {
        rc = ADSP_ENOMEMORY;
        goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_2;
    }
    for (int i=0; i<mem_map_regions_payload_ptr->num_regions; ++i)
    {
        phy_regions[i].shm_addr_lsw = (*ppRegion+i)->shm_addr_lsw;
        phy_regions[i].shm_addr_msw = (*ppRegion+i)->shm_addr_msw;
        phy_regions[i].mem_size = (*ppRegion+i)->mem_size_bytes;
    }

    if (0 == mem_map_regions_payload_ptr->property_flag)
    {
        //physical mapping
        if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_map(nMemMapClient,phy_regions,
                                                    mem_map_regions_payload_ptr->num_regions,
                                                    HMEM_CACHE_WRITEBACK_L2CACHEABLE,
                                                    eMemPool,
                                                    &mem_map_handle)))
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: Failed to map the physical memory, error code is 0x%x",rc );
            goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_3;
        }
    }
    else if (1 ==  mem_map_regions_payload_ptr->property_flag)
    {
        //virtual mapping
        if (ADSP_FAILED(rc = qurt_elite_memorymap_virtaddr_mem_map(nMemMapClient,phy_regions,
                                                    mem_map_regions_payload_ptr->num_regions,
                                                    HMEM_CACHE_WRITEBACK_L2CACHEABLE,
                                                    eMemPool,
                                                    &mem_map_handle)))
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: Failed to map the virual memory, error code is 0x%x",rc );
            goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_3;
        }

    }
    else
    {
        rc = ADSP_EBADPARAM;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: invalid property flag received in the payload, error code is 0x%x",rc );
        goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_3;
    }

    //store the memory map handle in the response payload
    pRespPayload->mem_map_handle         = mem_map_handle;

    /* send response */
    if (APR_EOK != (rc = elite_apr_if_async_send(hAprHandle, pRespPkt)))
    {
        rc = ADSP_EFAILED;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: failed to send the memory map response command, error code is  = 0x%x", rc);
        goto _elite_mem_shared_memory_map_regions_cmd_handler_bail_3;
    }

    qurt_elite_memory_free(phy_regions);
    elite_apr_if_free( hAprHandle,  pPkt);
    return rc;


_elite_mem_shared_memory_map_regions_cmd_handler_bail_3:
    qurt_elite_memory_free(phy_regions);
_elite_mem_shared_memory_map_regions_cmd_handler_bail_2:
   (void)elite_apr_if_free(hAprHandle,pRespPkt);
_elite_mem_shared_memory_map_regions_cmd_handler_bail_1:
    elite_apr_if_end_cmd(hAprHandle, pPkt, rc);
    return rc;

}

ADSPResult elite_mem_shared_memory_un_map_regions_cmd_handler(uint32_t nMemMapClient,
                                        elite_apr_handle_t hAprHandle,
                                        elite_apr_packet_t *pPkt)
{
    ADSPResult rc = ADSP_EOK;
    void * pPayload;
    avs_cmd_shared_mem_unmap_regions_t *pMemUnMapRegionsPayload;

    elite_apr_if_get_payload( (void**) &pPayload, pPkt );

    pMemUnMapRegionsPayload = (avs_cmd_shared_mem_unmap_regions_t *)pPayload;

    if (0 == pMemUnMapRegionsPayload->mem_map_handle)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: null memory map handle received in the unmap cmd payload");
        rc = ADSP_EBADPARAM;
        goto _elite_mem_shared_memory_un_map_regions_cmd_handler_bail_1;
    }

    if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_unmap(nMemMapClient,pMemUnMapRegionsPayload->mem_map_handle)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteMem: Failed to unmap the phyiscal memory, error code is 0x%x",rc );
    }

_elite_mem_shared_memory_un_map_regions_cmd_handler_bail_1:
    elite_apr_if_end_cmd(hAprHandle, pPkt, rc);
    return rc;

}

