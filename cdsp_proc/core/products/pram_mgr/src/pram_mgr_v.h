
/*==============================================================================
  FILE:         pram_mgr_clnt.c
  
  OVERVIEW:     Pram mgr driver internal declarations

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
				
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/pram_mgr/src/pram_mgr_v.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
==============================================================================*/

#ifndef PRAM_MGR_V_H
#define PRAM_MGR_V_H

#define MAX_PARTITIONS 5
#define PRAM_SIZE 0x6800  //26KB
                          //Need to hardcode this. Look into using devcfg.
                          //Cannot rely on the info from qdsp6.xml as the physpool has to be rounded to 4k boundary

#define MAX_NAMELEN 20
typedef struct partition {
   char name[MAX_NAMELEN];
   qurt_paddr_t base_pa;
   qurt_addr_t base_va;
   qurt_size_t size;
   int owner;
} partition_t;
typedef struct pram {
   char name[MAX_NAMELEN];
   qurt_mem_pool_t physpool;
   qurt_paddr_t base_pa;
   qurt_addr_t base_va;
   qurt_size_t max_size;
   partition_t partitions[MAX_PARTITIONS];
} pram_t;
/***************************/
/*Function declarations*****/
/***************************/
void pram_mgr_drv_init(void);

int pram_mgr_drv_get_partition_info(int client_handle, 
                                 char *name, 
                                 uint32 *addr, 
                                 uint32 *size);

int pram_mgr_drv_register_partition(int client_handle,
                                char *name, unsigned int virt_addr);

void pram_mgr_drv_deregister_partitions(int client_handle);

void pram_mgr_drv_register_err_handler(void);

void pram_mgr_drv_err_cb(void);
#endif /* PRAM_MGR_V_H */
