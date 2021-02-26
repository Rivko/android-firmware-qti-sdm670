
/*==============================================================================
  FILE:         pram_mgr_clnt.c
  
  OVERVIEW:     QDI client layer implementing the pram mgr API

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/pram_mgr/src/pram_mgr_clnt.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
==============================================================================*/

#include "pram_mgr.h"
#include "pram_mgr_qdi_v.h"
#include "qurt_memory.h"
#include "assert.h"
#include "err.h"
#include <stringl/stringl.h>

/* pram mgr client handle to communicate with guest os layer */
int pram_mgr_clnt_qdi_handle = -1;

int pram_mgr_clnt_pid;

qurt_mem_pool_t pram_phys_pool;
int pram_phys_pool_attach_done=0;

#define PAGE_ALIGN_MASK 0xFFF

void *pram_acquire_partition(char *name, uint32 *size)
{
   qurt_paddr_t partition_physaddr;
   qurt_size_t partition_size;
   qurt_mem_region_t region;
   qurt_mem_region_attr_t attr;
   qurt_paddr_t phys_addr;
   unsigned int virt_addr;
   unsigned int adjusted_virt_addr;
   unsigned int align_shift;
   unsigned int adjusted_size;
   int ret;

   ASSERT(pram_mgr_clnt_qdi_handle != -1);

   /* Avoid attaching multiple times */
   /* in case of multiple clients in the same PD */
   if(!pram_phys_pool_attach_done) {
      ret = qurt_mem_pool_attach(PRAM_PHYSPOOL_NAME, &pram_phys_pool);
      ASSERT(ret == QURT_EOK);
      pram_phys_pool_attach_done = 1; 
   }

   if(pram_mgr_invoke_get_partition_info(pram_mgr_clnt_qdi_handle, name, &partition_physaddr, &partition_size)!=0) {
      ERR_FATAL("QDI call to get partition information failed!", 0, 0, 0);
      return NULL;
   }

   /* Round physical address to 4KB aligned */
   phys_addr = (partition_physaddr & (~PAGE_ALIGN_MASK));
   /* Adjust the size accouting for the alignment */
   align_shift = partition_physaddr - phys_addr;
   adjusted_size = partition_size + align_shift;
   /* Round size to a page size multiple */
   adjusted_size = (adjusted_size + PAGE_ALIGN_MASK) & (~PAGE_ALIGN_MASK);


   /* Reserve the VA space of the required size*/
   qurt_mem_region_attr_init(&attr);
   qurt_mem_region_attr_set_mapping(&attr, QURT_MEM_MAPPING_NONE);
   
   ret = qurt_mem_region_create(&region, adjusted_size, pram_phys_pool, &attr);
   if(ret != QURT_EOK) {
      ERR_FATAL("qurt_mem_region_create failed! phys_addr=%p, size=%d, ret=%d", partition_physaddr, partition_size, ret);
      return NULL;
   }

   /* Get the allocated VA */
   ret = qurt_mem_region_attr_get(region, &attr);
   if(ret != QURT_EOK) {
      ERR_FATAL("qurt_mem_region_get_attr failed! phys_addr=%p, size=%d, ret=%d", partition_physaddr, partition_size, ret);
      return NULL;
   }
   
   qurt_mem_region_attr_get_virtaddr(&attr, &virt_addr);

   /* Create the VA to PA mapping */
   ret = qurt_mapping_create(virt_addr, phys_addr, adjusted_size, QURT_MEM_CACHE_NONE_SHARED, QURT_PERM_READ | QURT_PERM_WRITE);
   if(ret != QURT_EOK) {
      ERR_FATAL("qurt_mapping_create failed! phys_addr=%p, virt_addr=%d, ret=%d", partition_physaddr, virt_addr, ret);
      return NULL;
   }
   adjusted_virt_addr = virt_addr + align_shift;

   if(pram_mgr_invoke_register_partition(pram_mgr_clnt_qdi_handle, name, adjusted_virt_addr)!=0) {
      ERR_FATAL("QDI call to register for partition failed!", 0, 0, 0);
      return NULL;
   }

   /* Clear out the partition to avoid stale data */
   secure_memset((void *)(virt_addr + align_shift), 0, partition_size);

   *size = partition_size;
   return (void *)adjusted_virt_addr;
}

void pram_mgr_clnt_init(void)
{
   pram_mgr_clnt_pid = qurt_getpid();

   pram_mgr_clnt_qdi_handle = qurt_qdi_open(PRAM_MGR_DRIVER_NAME);
   if(pram_mgr_clnt_qdi_handle < 0)
   {
     ERR_FATAL("pram_mgr_clnt_init: qdi_open failed\n", 0, 0, 0);
     return;
   }
  /* if(pram_mgr_clnt_pid == 0) {

	   pram_mgr_clnt_qdi_handle = PRAM_DUMMY_ROOTPD_CLNT_ID;
      //Need to initialize dummy client id for Root PD usecases
      // ERR_FATAL("pram_mgr_clnt_init: pram_mgr_clnt not supported in guest os\n", 0, 0, 0);
	 
      return;
   }*/

   return;
}

#ifdef FEATURE_PRAM_MGR_CLNT_TEST

#include <stringl/stringl.h>

void *sensors_base_va;
uint32 sensors_size;
void *touch_base_va;
void *qmp_base_va;
uint32 touch_size;
uint32 qmp_size;
#define TEST_LOOPS 10

void pram_mgr_clnt_test_init(void)
{
   int i;
   for(i=0; i<TEST_LOOPS; i++) {
       sensors_base_va = pram_acquire_partition("SENSORS", &sensors_size);
       touch_base_va = pram_acquire_partition("TOUCH", &touch_size);
       qmp_base_va = pram_acquire_partition("QMP", &qmp_size);

       secure_memset((void *)((uint32)sensors_base_va+1024), 0x39, 1024);
       secure_memset((void *)((uint32)touch_base_va+1024), 0x48, 1024);
       secure_memset(qmp_base_va, 0x56, 1024);
 
       qurt_qdi_close(pram_mgr_clnt_qdi_handle);
 
       pram_mgr_clnt_init();
   }
}
#endif


