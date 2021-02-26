#ifndef DSM_META_H
#define DSM_META_H
/*===========================================================================

                                  D S M _ S H A R E D . H

GENERAL DESCRIPTION
  Data Services memory pool module.
  Support for meta data operations.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1/api/memory/dsm_meta.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/13/10    ag     Removed customer.h for CMI Phase 2 
11/08/06    mjb    Created
===========================================================================*/

#include "comdef.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSM_GET_META

DESCRIPTION

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
);

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
);

#define dsm_set_meta(item_ptr,meta_ptr,size) \
  dsmi_set_meta( item_ptr,meta_ptr,size,__FILE__,__LINE__)

#endif /* DSM_META_H */
