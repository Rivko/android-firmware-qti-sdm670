
/*==============================================================================
  FILE:         pram_mgr_drv.c
  
  OVERVIEW:     Pram mgr main driver

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/pram_mgr/src/pram_mgr_drv.c#3 $
$DateTime: 2017/10/25 10:03:42 $
$Author: pwbldsvc $
==============================================================================*/

#include "pram_mgr.h"
#include "pram_mgr_v.h"
#include "pram_mgr_qdi_v.h"
#include "qurt_memory.h"
#include "assert.h"
#include "err.h"
#include <stringl/stringl.h>


                          //Need to hardcode this. Look into using devcfg.
                          //Cannot rely on the info from qdsp6.xml as the physpool has to be rounded to 4k boundary

uint8 pram_dump[PRAM_SIZE];
uint8 g_pram_saved=-1;
extern qurt_mutex_t pram_mgr_mutex;

#define PRAM_MGR_LOCK() \
   qurt_rmutex_lock(&pram_mgr_mutex); \

#define PRAM_MGR_UNLOCK() \
   qurt_rmutex_unlock(&pram_mgr_mutex); \




extern pram_t g_pram;



void pram_mgr_drv_init(void)
{
   int err;
   qurt_mem_pool_attr_t pram_pool_attr;
   qurt_paddr_t addr;
   int i,sum;
   qurt_paddr_t next_pa_base;

    /* Initialize the mutex for driver code */
    qurt_rmutex_init(&pram_mgr_mutex);

   /* Get the PRAM physical address */
   err = qurt_mem_pool_attach(g_pram.name, &g_pram.physpool);
   ASSERT(err == QURT_EOK);
   err = qurt_mem_pool_attr_get(g_pram.physpool, &pram_pool_attr);
   ASSERT(err == QURT_EOK);
   err = qurt_mem_pool_attr_get_addr(&pram_pool_attr, 0, &addr); /*This pool has only one range */
   ASSERT(err == QURT_EOK);
   g_pram.base_pa = (uint32)addr;

   /* Ensure the partition sizes do not overflow the PRAM size */
   sum=0;
   for(i=0; i<MAX_PARTITIONS; i++) 
      sum+=g_pram.partitions[i].size;
   ASSERT(sum == PRAM_SIZE);

   /* Populate the base PAs of the partitions */
   next_pa_base = g_pram.base_pa;
   for(i=0; i<MAX_PARTITIONS; i++) {
      g_pram.partitions[i].base_pa = next_pa_base;
      next_pa_base += g_pram.partitions[i].size;
   } 
   pram_mgr_drv_register_err_handler();
}

int pram_mgr_drv_get_partition_info(int client_handle, 
                                 char *name, 
                                 uint32 *addr, 
                                 uint32 *size) 
{
   int i;
   char partition_name[MAX_NAMELEN];

   PRAM_MGR_LOCK();

   if(qurt_qdi_copy_from_user(client_handle, &partition_name, name, MAX_NAMELEN * sizeof(char))) {
      PRAM_MGR_UNLOCK();
      return -1;
   }
   partition_name[MAX_NAMELEN-1]='\0'; /*Make sure it is NULL terminated. */

   for(i=0; i<MAX_PARTITIONS; i++) 
      if(!strcmp(g_pram.partitions[i].name, partition_name)) /* TBD ** Use safe version of strcmp from stringl.h*/
             break;

   if(i==MAX_PARTITIONS) {
      PRAM_MGR_UNLOCK();
      return -1;
   }

   if(g_pram.partitions[i].owner != -1) {
       /* Partition already claimed by a different process */
       /* Disallow the request */
       PRAM_MGR_UNLOCK();
       return -1;
   }
	
   if(qurt_qdi_copy_to_user(client_handle, addr, &g_pram.partitions[i].base_pa, sizeof(uint32))) {
      PRAM_MGR_UNLOCK();
      return -1;
   }
   if(qurt_qdi_copy_to_user(client_handle, size, &g_pram.partitions[i].size, sizeof(uint32))) {
      PRAM_MGR_UNLOCK();
      return -1;
   }

   PRAM_MGR_UNLOCK();
   return 0;
}

int pram_mgr_drv_register_partition(int client_handle,
                                char *name, unsigned int virt_addr)
{
   int i;
   char partition_name[MAX_NAMELEN];
   unsigned int virt_addr_local;

   PRAM_MGR_LOCK();

   if(qurt_qdi_copy_from_user(client_handle, &partition_name, name, MAX_NAMELEN * sizeof(char))) {
      PRAM_MGR_UNLOCK();
      return -1;
   }
//   if(qurt_qdi_copy_from_user(client_handle, &virt_addr_local, virt_addr,sizeof(unsigned int))) {
//      PRAM_MGR_UNLOCK();
//      return -1;
//   }
   virt_addr_local = virt_addr;

   partition_name[MAX_NAMELEN-1]='\0'; /*Make sure it is NULL terminated. */

   for(i=0; i<MAX_PARTITIONS; i++) 
      if(!strcmp(g_pram.partitions[i].name, partition_name))  /* TBD ** Use safe version of strcmp from stringl.h */
             break;

   if(i==MAX_PARTITIONS) {
      /* No partitions found with the provided name */
      PRAM_MGR_UNLOCK();
      return -1;
   }

   if(g_pram.partitions[i].owner != -1) {
      /* Partition already claimed by a different process*/
      /* Disallow the request */
      PRAM_MGR_UNLOCK();
      return -1;
   }

   g_pram.partitions[i].owner = client_handle;
   g_pram.partitions[i].base_va = virt_addr_local;


   PRAM_MGR_UNLOCK();
   return 0; 
}

void pram_mgr_drv_deregister_partitions(int client_handle)
{
   int i;

   PRAM_MGR_LOCK();

   for(i=0; i<MAX_PARTITIONS; i++) 
      if(g_pram.partitions[i].owner == client_handle)
          g_pram.partitions[i].owner = -1;

   PRAM_MGR_UNLOCK();
   return;
}

#define PAGE_ALIGN_MASK 0xFFF
void pram_mgr_drv_register_err_handler(void)
{
   qurt_mem_region_t region;
   qurt_mem_region_attr_t attr;
   qurt_paddr_t phys_addr;
   unsigned int virt_addr;
   unsigned int align_shift;
   unsigned int adjusted_size;
   int ret;
   /* Round physical address to 4KB aligned */
   phys_addr = (g_pram.base_pa & (~PAGE_ALIGN_MASK));
   /* Adjust the size accouting for the alignment */
   align_shift = g_pram.base_pa - phys_addr;
   adjusted_size = g_pram.max_size + align_shift;
   /* Round size to a page size multiple */
   adjusted_size = (adjusted_size + PAGE_ALIGN_MASK) & (~PAGE_ALIGN_MASK);
   /* Reserve the VA space of the required size*/
   qurt_mem_region_attr_init(&attr);
   qurt_mem_region_attr_set_mapping(&attr, QURT_MEM_MAPPING_NONE);
   ret = qurt_mem_region_create(&region, adjusted_size, g_pram.physpool, &attr);
   ASSERT(ret == QURT_EOK); 
   /* Get the allocated VA */
   ret = qurt_mem_region_attr_get(region, &attr);
   ASSERT(ret == QURT_EOK); 
   qurt_mem_region_attr_get_virtaddr(&attr, &virt_addr);
   /* Create the VA to PA mapping */
   ret = qurt_mapping_create(virt_addr, phys_addr, adjusted_size, QURT_MEM_CACHE_NONE_SHARED, QURT_PERM_READ | QURT_PERM_WRITE);
   ASSERT(ret == QURT_EOK); 
   g_pram.base_va = virt_addr + align_shift;
   /* Register error callback */
   err_crash_cb_register(pram_mgr_drv_err_cb);   
   g_pram_saved=0;
}
void pram_mgr_drv_err_cb(void)
{
   memscpy((void *)pram_dump, PRAM_SIZE, (void *)g_pram.base_va, PRAM_SIZE);
   g_pram_saved = 1;
   return;
}

#ifdef FEATURE_PRAM_MGR_CLNT_TEST
void *buses_base_va;
uint32 buses_pram_size;
void *gsi_base_va;
uint32 gsi_pram_size;
int buses_base_pa;
int gsi_base_pa;



void pram_mgr_clnt_rootpd_test(void){

	buses_base_va = pram_acquire_partition("BUSES", &buses_pram_size);
	gsi_base_va = pram_acquire_partition("GPI", &gsi_pram_size);
	//gsi_base_pa = pram_mgr_lookup_phys_addr(PRAM_DUMMY_ROOTPD_CLNT_ID, (qurt_addr_t)gsi_base_va);
	gsi_base_pa = pram_mgr_lookup_phys_addr_by_name("BUSES", (qurt_addr_t)gsi_base_va);
	//buses_base_pa = pram_mgr_lookup_phys_addr(PRAM_DUMMY_ROOTPD_CLNT_ID, (qurt_addr_t)buses_base_va);
	buses_base_pa = pram_mgr_lookup_phys_addr_by_name("GPI", (qurt_addr_t)buses_base_va);


}

#endif
