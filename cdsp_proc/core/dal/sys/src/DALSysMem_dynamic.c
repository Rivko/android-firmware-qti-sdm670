/*==============================================================================
@file  DALSYSMem_dynamic.c

DALSYS dynamic memory implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALSysInt.h"
#include <qurt.h>

/*==============================================================================
          Structures
 ==============================================================================*/

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS
==============================================================================*/
#ifndef DALSYS_MEM_64BIT_PHYS_ADDR

#define QMEM_ATTR_SET_PHYSADDR( attr, paddr ) \
   qurt_mem_region_attr_set_physaddr(  attr, (qurt_paddr_t)paddr )
#define QMEM_ATTR_GET_PHYSADDR( attr, ppaddr ) \
   *ppaddr = 0; \
   qurt_mem_region_attr_get_physaddr(  attr, (qurt_paddr_t *)ppaddr )
#define QMEM_MAP_STATIC_QUERY( vaddr, paddr, len, cache, perm )\
   qurt_mem_map_static_query( vaddr, (qurt_paddr_t)paddr, len, cache, perm )
#define QMEM_MAPPING_CREATE( vaddr, paddr, len, cache, perm ) \
   qurt_mapping_create( vaddr, (qurt_paddr_t)paddr, len, cache, perm )
#define QMEM_MAPPING_REMOVE( vaddr, paddr, len ) \
   qurt_mapping_remove( vaddr, (qurt_paddr_t)paddr, len )

#else

#define QMEM_ATTR_SET_PHYSADDR( attr, paddr ) \
   qurt_mem_region_attr_set_physaddr_64(  attr, paddr )
#define QMEM_ATTR_GET_PHYSADDR( attr, ppaddr ) \
   qurt_mem_region_attr_get_physaddr_64(  attr, ppaddr )
#define QMEM_MAP_STATIC_QUERY( vaddr, paddr, len, cache, perm )\
   qurt_mem_map_static_query_64( vaddr, paddr, len, cache, perm )
#define QMEM_MAPPING_CREATE( vaddr, paddr, len, cache, perm ) \
   qurt_mapping_create_64( vaddr, paddr, len, cache, perm )
#define QMEM_MAPPING_REMOVE( vaddr, paddr, len ) \
   qurt_mapping_remove_64( vaddr, paddr, len )

#endif

/*============================================================================n
      Constants and Macros
==============================================================================*/

/*==============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static DALResult create_qurt_mem_region( 
      qurt_mem_pool_t mem_pool, qurt_mem_cache_mode_t cache_attribs, 
      qurt_mem_mapping_t mapping, const DALSYSMemReq *pMemReq, 
      DALSYSMemInfoEx *pMemInfo, qurt_mem_region_t * p_qmem_region )
{
   qurt_mem_region_attr_t mem_attribs;
   qurt_mem_region_attr_init (&mem_attribs); 
   qurt_mem_region_attr_set_cache_mode(&mem_attribs, cache_attribs);
   qurt_mem_region_attr_set_mapping(&mem_attribs, mapping);
   qurt_mem_region_attr_set_bus_attr(&mem_attribs, DALSYS_MEM_BUS_ATTR);

   if ( DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemReq->memInfo.physicalAddr )
   {
      QMEM_ATTR_SET_PHYSADDR(&mem_attribs, pMemReq->memInfo.physicalAddr);
   }

   if(QURT_EOK == qurt_mem_region_create(p_qmem_region,
      pMemReq->memInfo.size, mem_pool, &mem_attribs))
   {
      // we need to read addresses and size attributes because memory region
      // addresses and size can be modified to match alignment
      // and size of memory mappings
      qurt_mem_region_attr_get(*(p_qmem_region), &mem_attribs );
      qurt_mem_region_attr_get_virtaddr(&mem_attribs, 
         (qurt_addr_t *)&(pMemInfo->virtualAddr)); 
      qurt_mem_region_attr_get_size(&mem_attribs, 
         (qurt_size_t *)&(pMemInfo->size));

      // if memory region with virt-to-phys mapping is created,
      // physical address is updated from the memory region, otherwise
      // physical address is rounded-down according to the region size
      if (QURT_MEM_MAPPING_NONE != mapping)
      {
         QMEM_ATTR_GET_PHYSADDR(&mem_attribs, &(pMemInfo->physicalAddr)); 
      }
      return DAL_SUCCESS;
   }
   return DAL_ERROR;
}


DALResult DALSYS_MemAllocQurtRegion( const DALSYSMemReq *pMemReq,
      DALSYSMemInfoEx *pMemInfo, qurt_mem_region_t *p_qmem_region )
{
   static qurt_mem_pool_t hwio_phys_pool = NULL;
   DALResult result;
   qurt_mem_cache_mode_t qmem_cache;

   if (DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemReq->memInfo.virtualAddr)
   {
      // no support for memory region creation with a 
      // specified virtual address
      DALSYS_LOG_ERROR_EVENT("Cannot specify virtual address- pMemReq:0x%x", 
            pMemReq);
      return DAL_ERROR;
   }

   // allocate from HWIO pool
   if (NULL == hwio_phys_pool)
   {
      // need to attach to HWIO pool if this is the first allocation
      if (QURT_EOK != qurt_mem_pool_attach ("HWIO_POOL", &hwio_phys_pool))
      {
         DALSYS_LOG_ERROR_EVENT("Cannot attach to HWIO pool");
         return DAL_ERROR;
      }
   }

   // set cache mode
   switch (pMemReq->memInfo.cacheType)
   {
      case DALSYS_MEM_CACHE_DEVICE:
         // change to QURT_MEM_CACHE_DEVICE once available
         qmem_cache = QURT_MEM_CACHE_NONE;
         break;

      case DALSYS_MEM_CACHE_NONE:
         qmem_cache = QURT_MEM_CACHE_NONE_SHARED;
         break;

      case DALSYS_MEM_CACHE_WRITEBACK:
         qmem_cache = QURT_MEM_CACHE_WRITEBACK;
         break;

      case DALSYS_MEM_CACHE_WRITETHROUGH:
         qmem_cache = QURT_MEM_CACHE_WRITETHROUGH;
         break;

      default:
         DALSYS_LOG_ERROR_EVENT("Unsupported cache type: 0x%x", 
               pMemReq->memInfo.cacheType); 
         return DAL_ERROR;
   }

   // copy memory info
   *pMemInfo = pMemReq->memInfo;

   if (DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemReq->memInfo.physicalAddr)
   {
      int qurt_ret;
      qurt_addr_t qmemVirtualAddr;
      qurt_perm_t qmem_perms;

      // permission
      if (pMemReq->memInfo.permission < DALSYS_MEM_PERM_INVALID)
      {
         qmem_perms = (qurt_perm_t)pMemReq->memInfo.permission;
      }
      else
      {
         DALSYS_LOG_ERROR_EVENT("Unsupported permission: 0x%x", 
               pMemReq->memInfo.permission); 
         return DAL_ERROR;
      }

      // attempt to find memory from statically defined memory regions
      qurt_ret = QMEM_MAP_STATIC_QUERY(&qmemVirtualAddr,
                 (pMemReq->memInfo.physicalAddr), pMemReq->memInfo.size, 
                 qmem_cache, qmem_perms);

      switch(qurt_ret)
      {
         case QURT_EOK: // memory region found
            *(p_qmem_region) = 0;  //no underlying qmem_region, memory mapped statically
            pMemInfo->virtualAddr = qmemVirtualAddr;
            result = DAL_SUCCESS; //have correct vaddr returned in second argument
            DALSYS_LOG_INFO_EVENT("Static memory region found");
            break;
         case QURT_EMEM: // memory region found but attributes don't match
            DALSYS_LOG_WARNING_EVENT(
               "memory region found but attributes not match- pMemReq:0x%x",
               pMemReq );
            result = DAL_ERROR;
            break;
         default: // memory region is not found - create it
            result = create_qurt_mem_region( hwio_phys_pool, qmem_cache, 
               QURT_MEM_MAPPING_PHYS_CONTIGUOUS, pMemReq, pMemInfo, 
               p_qmem_region );
            break;
      }

      if (DAL_SUCCESS != result)
      {
         // unable to find or allocate from HWIO pool
         // allocate virtual memory and map to specified physical memory region
         // below pool parameter is basically ignored by QuRT as no 
         // physical memory is mapped, however it must still be a valid pool handle
         result = create_qurt_mem_region( hwio_phys_pool, qmem_cache, 
            QURT_MEM_MAPPING_NONE, pMemReq, pMemInfo, p_qmem_region);

         if (DAL_SUCCESS == result)
         {
            qurt_ret = QMEM_MAPPING_CREATE( pMemInfo->virtualAddr, 
                  pMemReq->memInfo.physicalAddr, pMemInfo->size, qmem_cache, 
                  qmem_perms);
            if (QURT_EOK != qurt_ret)
            {
               qurt_mem_region_delete(*p_qmem_region);
               result = DAL_ERROR;
            }
            DALSYS_LOG_INFO_EVENT("Unspecified memory region created");
         }
      }
      else
      {
         DALSYS_LOG_INFO_EVENT("HWIO memory region created");
      }
   }
   else
   {
      // physical address is not specified
      // allocate from DEFAULT pool
      result = create_qurt_mem_region( qurt_mem_default_pool, qmem_cache,
         QURT_MEM_MAPPING_VIRTUAL, pMemReq, pMemInfo, p_qmem_region );
      DALSYS_LOG_INFO_EVENT("Default memory region created");
   }
   return result;
}

void DALSYS_MemFreeQurtRegion( DALSYSMemInfoEx const *pMemInfo, 
      qurt_mem_region_t qmem_region )
{
   qurt_mem_region_attr_t attr;

   if (NULL == qmem_region) return;

   if (QURT_EOK != qurt_mem_region_attr_get(qmem_region, &attr))
   {
      DALSYS_LOG_WARNING_EVENT("Unable to get memory attributes");
      return;
   }

   qurt_mem_region_delete(qmem_region);

   if (QURT_MEM_MAPPING_NONE == attr.mapping_type)
   {
      QMEM_MAPPING_REMOVE( pMemInfo->virtualAddr,
         (qurt_addr_t)pMemInfo->physicalAddr, pMemInfo->size);
   }
}
