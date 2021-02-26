#ifndef _SMEM_V_H_
#define _SMEM_V_H_

/*===========================================================================

                    S H A R E D   M E M O R Y

DESCRIPTION
    Defines the AU public api that is used to allocate memory in physical
    SMEM.  This API deal strictly with void * to reduce dependencies.

 Copyright (c)  2009-2013,2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/11/13   an      added smem_get_num_hosts
08/27/13   pa      Adding SMEM_ALLOC_FLAG*.
08/16/13   bt      Add smem_get_part_sz for internal clients.
06/25/13   pa      Added smem_alloc_add_table_entry for SMEM backwards 
                   compatibility. 
05/11/12   bm      Added smem_alloc_rpm() API to allocate from MSG RAM.
01/20/12   bm      Add API smem_set_ext_mem_region(), smem_alloc_ext_mem()
                   to support memory allocation from outside SMEM for RPM.
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: adding Spinlocks
                   Region base address initialization
02/17/11   bm      Added API declaration for 'smem_map_base_va'
01/06/11   bt      Added definition for size of first SMEM region.
12/08/10   rs      Added generic SMEM memory barrier
03/02/10   tl      Added debug-smem-at-boot support
11/25/09   rs      CMI updates 
11/08/09   hwu     Remove redundant structure defines. 
04/20/09   bfc     Initial version...Split smem.h into AU public and AU
                   private headers

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "smem_type.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/* This is used for offset purposes in the absence of the Prom Comm SMEM 
 * definitions.  This value replaces 
 * SMEM_PROC_COMM_NUM_PROCS * sizeof(smem_proc_comm_data_type) */
#define SMEM_MEM_FIRST_SIZE_BYTES     (4 * 16)

#define SMEM_DIAG_BUFFER_LEN           0xC8

/** Flags for use with smem_alloc_ex()/
 ** For use by Mproc code only. */
#define SMEM_ALLOC_FLAG_CACHED          0x80000000 /**< Allocate from cached 
                                                    **  heap */
#define SMEM_ALLOC_FLAG_PARTITION_ONLY  0x40000000 /**< Allocate from edge 
                                                    **  partition only. */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smem_alloc_add_table_entry
===========================================================================*/
/**
  Add an entry to the SMEM allocation table without allocating any memory and 
  without updating the heap info structure.

  Caller must ensure that there is not currently an allocation at this location.

  This function is designed for use with the backwards compatibility mode 
  used during the roll out of SMEM partitioning.

  @param[in]    smem_type   SMEM ID to use for the allocation
  @param[in]    buffer      pointer to the existing allocation
  @param[in]    size        size of the existing allocation

  @return
  None.

  @dependencies
  SMEM must be initialized before calling.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_alloc_add_table_entry
( 
  smem_mem_type smem_type, 
  void * buffer, 
  uint32 size 
);

/*===========================================================================
FUNCTION      smem_get_partition_sz
===========================================================================*/
/**
  Return the size of the SMEM partition between the specified remote_host and
  the local processor.

  @param[in] remote_host  The remote processor's smem_mem_type.

  @return
  The size of the SMEM partition, if any, shared with remote_host.
  0 if no partition for this edge has been reserved.
*/
/*=========================================================================*/
uint32 smem_get_partition_sz( smem_host_type remote_host );

/*===========================================================================
FUNCTION      smem_get_num_hosts
===========================================================================*/
/**
  Return the maximum nuber of hosts. It should not be less than
  SMEM_NUM_HOSTS.

  @return
  The maximum nuber of hosts.
*/
/*=========================================================================*/
uint32 smem_get_num_hosts( void );

#endif /* _SMEM_V_H_ */
