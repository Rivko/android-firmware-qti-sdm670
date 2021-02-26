
#include "qurt_elite.h"

ADSPResult HwdDevCfg_GetVirtAddress(qurt_mem_region_t *pQmem_region, uint32 phyAddr, uint32 regSize, uint32 *pVirtAddr)
{
   ADSPResult status = ADSP_EOK;
   qurt_mem_pool_t hwio_pool;

   memset((void *)pQmem_region,0,sizeof(qurt_mem_region_t));

   size_t   page_size = (size_t) regSize;
   qurt_mem_cache_mode_t cache_attribs =  QURT_MEM_CACHE_NONE;
   //qurt_perm_t perm = (qurt_perm_t) (QURT_PERM_READ | QURT_PERM_WRITE | QURT_PERM_EXECUTE);

   qurt_perm_t perm = (qurt_perm_t) (QURT_PERM_READ | QURT_PERM_WRITE);

   if ( ADSP_EOK !=  (status = qurt_mem_map_static_query((qurt_addr_t *)pVirtAddr,
                              phyAddr,
                              page_size,
                              cache_attribs,
                              perm)))
   {
      if(ADSP_EOK != (status = qurt_mem_pool_attach("ADSP_DRIVER_POOL", &hwio_pool)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HwdDevCfg_GetVirtAddress: attach to driver pool failed\n");	  
      }
      else
      {
         qurt_mem_region_attr_t hwio_attr;

         qurt_mem_region_attr_init( &hwio_attr);
         qurt_mem_region_attr_set_cache_mode( &hwio_attr, QURT_MEM_CACHE_NONE);
         qurt_mem_region_attr_set_mapping  ( &hwio_attr, QURT_MEM_MAPPING_PHYS_CONTIGUOUS );
         qurt_mem_region_attr_set_physaddr ( &hwio_attr, phyAddr );
         
         //#ifdef SIM
         /* 
            Without QURT_MEM_REGION_SHARED, memory is not global 
            Then, qurt_mem_map_static_query() API cannot locate that in MPD build.
            Once qurt_mem_map_static_query() is fixed, this could be removed
         */
         qurt_mem_region_attr_set_type(&hwio_attr, QURT_MEM_REGION_SHARED);
         //#endif

         /* create the memory region */
         if (ADSP_EOK != (status = qurt_mem_region_create(pQmem_region,
                               regSize,
                               hwio_pool,
                              &hwio_attr )))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HwdDevCfg_GetVirtAddress: create mem region failed (addr 0x%x) (size 0x%x)\n", phyAddr, page_size);
            return status;			
         }

         if (ADSP_EOK != (status = qurt_mem_region_attr_get(*pQmem_region, &hwio_attr)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HwdDevCfg_GetVirtAddress: get mem region attr failed (addr 0x%x) (size 0x%x)\n", phyAddr, page_size);
         }
         else
         {
            unsigned int addr = 0;
            qurt_mem_region_attr_get_virtaddr(&hwio_attr, &addr);
            *pVirtAddr = (uint32) addr;
         }
	   }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HwdDevCfg_GetVirtAddress: already mampped statically 0x%x\n", (unsigned int)phyAddr);  
   }
   return status;
}


ADSPResult HwdDevCfg_DestoryVirtAddress(qurt_mem_region_t qmem_region)
{
   ADSPResult status = ADSP_EOK;

   if(qmem_region)
   {
      if(QURT_EOK != (status = qurt_mem_region_delete(qmem_region)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HwdDevCfg_DestoryVirtAddress: Failed to delete memory region [status:%d]", status);
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HwdDevCfg_DestoryVirtAddress: no qmem_region to destory");
   }

   return status;
}

ADSPResult HwdDevCfg_GetVirtAddress_Statically_Mapped(
            qurt_mem_cache_mode_t cache_attribs,
            uint32 phyAddr,
            uint32 *pVirtAddr,
            qurt_perm_t perm)
{
   ADSPResult status = ADSP_EOK;

   if ( ADSP_EOK !=  (status = qurt_mem_map_static_query(
                              (qurt_addr_t *)pVirtAddr,
                              phyAddr,
                              0x1000, //Minimum page size
                              cache_attribs,
                              perm)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "This memsection corresponding to phys addr = 0x%x is not statically mapped", (unsigned int)phyAddr);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HwdDevCfg_GetVirtAddress: already " \
         "mampped statically. phys addr queried = 0x%x, virtual addr = 0x%x",
         (uint32)phyAddr, (uint32)*pVirtAddr);
   }

   return status;
}

