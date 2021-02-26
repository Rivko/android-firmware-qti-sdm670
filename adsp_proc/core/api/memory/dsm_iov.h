#ifndef DSM_IOV_H
#define DSM_IOV_H
/*===========================================================================

                             D S M _ I O V . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#define DSM_IOV_MAJOR_VER_NO 1
#define DSM_IOV_MINOR_VER_NO 0

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1/api/memory/dsm_iov.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
01/13/10    ag     Remove customer.h for CMI Phase 2. 
09/25/09    ag     Remove the supression and renamed the function. 
09/17/09    ag     Supressing a lint warning.
01/01/05    pjb    Created.
===========================================================================*/

#include "comdef.h"
#include "dsm_item.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

struct dsm_iov {
  byte    * iov_base;		/* starting address of buffer */
  uint32    iov_len;		/* size of the buffer in bytes */
};

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSM_IOV()

DESCRIPTION
  This function will fill in the provided IOVEC array with the data_ptr's
  and lengths of the items in a dsm packet.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to the head of the packet to extract from.
  vector - An array of dsm_iov's that will be populated with the data_ptrs 
           and used fields from the packet provided.
  vector_length - The length of the previous vector. 

RETURN VALUE
  An integer indicating the number of vector elements used. 

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_iov
(
  dsm_item_type * pkt_head_ptr, 
  struct dsm_iov * vector,
  uint32 * vector_length,
  const char * file,
  uint32  line
);

#define dsm_iov_fill(pkt_head_ptr,vector,vector_length) \
       dsmi_iov(pkt_head_ptr,vector,vector_length,__FILE__,__LINE__) 


/*===========================================================================
FUNCTION DSM_IOV_NUM_BUFFERS()

DESCRIPTION
  This function will return the length of a DSM chain.  This is useful for
  determining the length of a IOV needed to hold this item.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to the head of the packet to extract from.

RETURN VALUE
  An integer indicating the number of buffers in the dsm item.

SIDE EFFECTS
  None
===========================================================================*/

uint32 dsm_iov_num_buffers
(
  dsm_item_type * pkt_head_ptr
);

#define dsm_num_buffers(pkt_head_ptr) dsm_iov_num_buffers(pkt_head_ptr)

                                                  
#endif /* DSM_IOV_H */
