/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ I O V . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_reg_mempool_events()
    Getister the passed fucntion pointer to be later invoked when the
    specified memory events happen for the specified memory pool.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/memory/dsm/src/dsm_iov.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/22/10    ag     DSM CMI Changes.
09/25/09    ag     Removed the supression. 
09/18/09    ag     Supressed a lint warning caused due to same name of struct 
                   and a macro. Both are used externally. 
04/28/09    ag     Replaced assert.h with amssassert.h
01/23/07    mjb    Featurized call to dsmi_touch_item.
07/07/06    mjb    Removed duplicate ASSERT from dsm_iov, now touches packet.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#define FEATURE_DSM_WM_CB
#include "dsm_iov.h"
#include "dsmi.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "amssassert.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

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
  The DSM packet will be "touched" because this API opens up the data
  to being altered.
===========================================================================*/
uint32 dsmi_iov
(
  dsm_item_type * pkt_head_ptr,
  struct dsm_iov * vector,
  uint32 * vector_length,
  const char * file,
  uint32 line
)
{
  uint32 index = 0;
  uint32 cnt = 0;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(pkt_head_ptr != NULL) 
  {
    dsmi_verify_packet(pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */
  
  /*-----------------------------------------------------------------
    Iterate across the packet and count used size 
  -----------------------------------------------------------------*/
  while(pkt_head_ptr != NULL && index < *vector_length)
  {
    if(pkt_head_ptr->references  == 0)
    {
      ERR_FATAL("Attempt to copy from a freed packet",0,0,0);
    }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_IOV_DUP
    if(pkt_head_ptr->references > 1 || pkt_head_ptr->dup_ptr != NULL)
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Copying from a duplicate item, or a duplicated item");
    }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_IOV_DUP */

    dsmi_touch_item(pkt_head_ptr,file,line);
          
    vector[index].iov_base = pkt_head_ptr->data_ptr;
    vector[index].iov_len = pkt_head_ptr->used;
    cnt += pkt_head_ptr->used;
    index ++;
    pkt_head_ptr = pkt_head_ptr->pkt_ptr;
  }

  *vector_length = index;
  return cnt; 
}

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

uint32 dsm_iov_num_buffers(dsm_item_type * pkt_head_ptr)
{
  uint32 cnt = 0;
  while(pkt_head_ptr != NULL)
  {
    cnt ++;
    pkt_head_ptr = pkt_head_ptr->pkt_ptr;
  }
  return cnt;
}
