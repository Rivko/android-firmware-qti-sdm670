/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ M E T A . C

GENERAL DESCRIPTION
  Data Services memory pool module.
  Support for metadata operations.

EXTERNALIZED FUNCTIONS
   

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1/memory/dsm/src/dsm_meta.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/22/10    ag     DSM CMI Changes.
04/27/09    ag     Replaced assert.h with amssassert.h
11/08/06    mjb    Created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_meta.h"
#include "dsm_pool.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSM_GET_META

DESCRIPTION
  Gets meta data from a DSM item.

DEPENDENCIES

PARAMETERS
  item_ptr - DSM item to get metadata from
  meta_ptr - Space to copy metadata into provided by user
  size     - Size of metadata

RETURN VALUE
  None

SIDE EFFECTS
  Copies metadata into meta_ptr, will assert if not a dup, or size is too
  big.
===========================================================================*/

void
dsm_get_meta
(
  dsm_item_type * item_ptr,
  void * meta_ptr,
  uint32 size
)
{
  dsm_pool_mgmt_table_type *table;

  DSM_ASSERT( item_ptr != NULL && meta_ptr != NULL );
  DSM_ASSERT( DSM_POOL_ITEM_SIZE(item_ptr->pool_id) == 0 );

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(item_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(item_ptr->pool_id);
#endif

  table = (dsm_pool_mgmt_table_type *)item_ptr->pool_id;
  DSM_ASSERT( table->pad_size >= size && size > 0 );
  
  memcpy( meta_ptr, (uint8*)(item_ptr) - size, size);

} /* dsm_get_meta */

/*===========================================================================
FUNCTION DSM_SET_META

DESCRIPTION
  Sets meta data for a DSM item.
  
DEPENDENCIES

PARAMETERS
  item_ptr - DSM item to set metadata for
  meta_ptr - Metadata info provided by user
  size     - Size of metadata to set

RETURN VALUE
  None

SIDE EFFECTS
  Copies users metadata into DSM item.
===========================================================================*/

void
dsmi_set_meta
(
  dsm_item_type * item_ptr,
  void * meta_ptr,
  uint32 size,
  const char * file,
  uint32 line
)
{
  
  dsm_pool_mgmt_table_type *table;

  DSM_ASSERT( item_ptr != NULL && meta_ptr != NULL );
  DSM_ASSERT( DSM_POOL_ITEM_SIZE(item_ptr->pool_id) == 0 );

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(item_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(item_ptr->pool_id);
#endif

  table = (dsm_pool_mgmt_table_type *)item_ptr->pool_id;
  DSM_ASSERT( table->pad_size >= size && size > 0 );
  
  memcpy( (uint8*)(item_ptr) - size,  meta_ptr, size );

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_touch_item(item_ptr, file, line);
#endif /* FEATURE_DSM_MEM_CHK */
  
} /* dsm_set_meta */
