#ifndef DSM_INIT_H
#define DSM_INIT_H
/*===========================================================================

                                  D S M _ I N I T. H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1/api/memory/dsm_init.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/11    mpa    Remove customer.h include
01/01/05    pjb    Created
===========================================================================*/

#include "comdef.h"
#include "dsm_pool.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Type for identifying a particular DSM memory pool
---------------------------------------------------------------------------*/
#define DSM_DS_SMALL_ITEM_POOL ((dsm_mempool_id_type)(&dsm_ds_small_item_pool))
extern dsm_pool_mgmt_table_type dsm_ds_small_item_pool;

#define DSM_DS_LARGE_ITEM_POOL ((dsm_mempool_id_type)(&dsm_ds_large_item_pool))
extern dsm_pool_mgmt_table_type dsm_ds_large_item_pool;

#define DSM_DUP_ITEM_POOL ((dsm_mempool_id_type)(&dsm_dup_item_pool))
extern dsm_pool_mgmt_table_type dsm_dup_item_pool;

/*---------------------------------------------------------------------------
  Definitions for few, many and do not exceed counts used in buffer based
  flow control.  Dont_Exceed is based on the minimum number of buffers
  required by the system. All incoming data from Rm or Um is dropped at this
  point.  Few Mark is when the system starts flow controlling the Um and Rm
  interfaces to stop new data from coming into the system.  Many Mark is when
  the system reallows new data to come into the system from the Um or the Rm
  interface.  These are empirical numbers based on various test scenarios.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Small and large items have separate flow points.  The small item and large
  item numbers are approximately in the same ratio.
---------------------------------------------------------------------------*/

#define DSM_DS_SMALL_ITEM_SIZ                                               \
    (DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))

#define DSM_DS_SMALL_ITEM_CNT                                               \
    (DSM_POOL_ITEM_CNT(DSM_DS_SMALL_ITEM_POOL))

/*---------------------------------------------------------------------------
  Size, Count and count for different memory marks/levels for LARGE items. 
  The significants of the counts DONT_EXCEED, TRANSPORT_FEW, FEW and MANY
  is same as described above for small items.

  Sizing for Large Memory Pool items. Header size defines enough space for
  worst case TCP/IP/PPP header.  This should be:

    Default Max MSS + TCP Header Size + MAX TCP Options Size + IP Header Size
    + MAX IP Options Size + MAX PPP Header Size + PPP Checksum Size.

  The large item count needs to be increased if maximum-size SSL records
  are processed.
---------------------------------------------------------------------------*/

#define DSM_DS_LARGE_ITEM_SIZ                                               \
    (DSM_POOL_ITEM_SIZE(DSM_DS_LARGE_ITEM_POOL)) 
#define DSM_DS_LARGE_ITEM_CNT                                               \
    (DSM_POOL_ITEM_CNT(DSM_DS_LARGE_ITEM_POOL))

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DUP items 
---------------------------------------------------------------------------*/

#define DSM_DUP_ITEM_SIZ                                                    \
    (DSM_POOL_ITEM_SIZE(DSM_DUP_ITEM_POOL)) 
#define DSM_DUP_ITEM_CNT                                                    \
    (DSM_POOL_ITEM_CNT(DSM_DUP_ITEM_POOL))

#define DSMI_DUP_ITEM_GET_RLC_DNE_MARK() 300//dsmi_dup_item_rlc_dne_mark

/*---------------------------------------------------------------------------
  This MACRO will return the memory pool ID for the DS memory pool (DS_SMALL
  or DS_LARGE) that is >= the passed size parameter.  This MACRO should only
  be used for the DS Small & DS Large item pools.
---------------------------------------------------------------------------*/
#define DSM_DS_POOL_SIZE(size)                                              \
  (dsm_mempool_id_type)                                                     \
  ((DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL) >= (uint16)(size)) ?         \
   (DSM_DS_SMALL_ITEM_POOL) : (DSM_DS_LARGE_ITEM_POOL))

/*---------------------------------------------------------------------------
  This MACRO will return the pool ID of the "other" memory pool. i.e., the
  macro returns LARGE pool_id if the user supplied SMALL pool_id as 
  paramter and LARGE pool_id, otherwise.  This MACRO should only be used
  for the DS Small & DS Large item pools.
---------------------------------------------------------------------------*/
#define DSM_DS_OTHER_POOL_ID(pool_id)                                       \
  ((dsm_mempool_id_type)                                                    \
   (pool_id==DSM_DS_SMALL_ITEM_POOL?                                        \
     DSM_DS_LARGE_ITEM_POOL:DSM_DS_SMALL_ITEM_POOL))

/*---------------------------------------------------------------------------
  DUP macros to dup from the dup item pool defined here.
---------------------------------------------------------------------------*/

#define dsm_dup_packet(dup_ptr, src_ptr, offset, cnt)                       \
        dsmi_dup_packet(dup_ptr, DSM_DUP_ITEM_POOL,                         \
                        src_ptr, offset, cnt,__FILE__,__LINE__)

#define dsm_dup_external_buffer( dup_ptr, src_ptr, cnt)                     \
       dsmi_dup_external_buffer( dup_ptr, DSM_DUP_ITEM_POOL,                \
                                 src_ptr, cnt,__FILE__,__LINE__)

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/
/*================ MEMORY MANAGMENT FUNCTION ==============================*/
/*===========================================================================
FUNCTION DSM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool. It should be
  called once upon system startup. All the memory items are initialized and
  put onto their respective free queues.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsm_init
( 
  void 
);

#endif /* DSM_INIT_H */

