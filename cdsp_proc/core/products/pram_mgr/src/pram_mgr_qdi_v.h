

/*==============================================================================
  FILE:         pram_mgr_qdi_v.h
  
  OVERVIEW:     Macro definitions for Pram mgr QDI 

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/pram_mgr/src/pram_mgr_qdi_v.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
==============================================================================*/

#ifndef PRAM_MGR_QDI_V_H
#define PRAM_MGR_QDI_V_H

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt.h"
#include "pram_mgr.h"

#define PRAM_PHYSPOOL_NAME "SSC_PRAM"

#define PRAM_MGR_DRIVER_NAME   "/dev/pram_mgr"

#define PRAM_MGR_QDI_GET_PARTITION_INFO    (0+QDI_PRIVATE)
#define PRAM_MGR_QDI_REGISTER_PARTITION    (1+PRAM_MGR_QDI_GET_PARTITION_INFO)



static inline int pram_mgr_invoke_get_partition_info(
                                                int handle,
                                                char *name,
                                                qurt_paddr_t *base_pa,
                                                qurt_size_t *size)
{
     return qurt_qdi_handle_invoke(handle, PRAM_MGR_QDI_GET_PARTITION_INFO, name, base_pa, size);
}

static inline int pram_mgr_invoke_register_partition(
                                                int handle,
                                                char *name,
                                                unsigned int virt_addr)
{
     return qurt_qdi_handle_invoke(handle, PRAM_MGR_QDI_REGISTER_PARTITION, name, virt_addr);
}



#endif /* PRAM_MGR_QDI_V_H */
