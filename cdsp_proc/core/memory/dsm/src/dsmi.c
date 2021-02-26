/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M I . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/memory/dsm/src/dsmi.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/30/11   ejv     Remove AEEstd.h.
03/22/11    rj     Added debug code
03/08/11    ag     Remove FEATURE_DSM_WM_CB
09/17/09    ag     Fixed lint warnings.
04/27/09    ag     Replaced assert.h with amssassert.h
01/05/05    pjb    Created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_pool.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "amssassert.h"
#ifdef FEATURE_DSM_MEM_CHK
#include <stringl/stringl.h>
#endif /* FEATURE_DSM_MEM_CHK */

#ifdef FEATURE_DSM_NATIVE_ASSERT
#define DSM_ASSERT( cond )      assert( cond )
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Pointer to list of memory pools */

dsm_pool_mgmt_table_type * dsm_pool_head_ptr;

/* ------------- Runtime Debug Features ------------- */

/* Only fill freed items with AA if this is non-zero. */
#ifdef FEATURE_DSM_MEM_CHK
#ifdef FEATURE_DSM_MEM_CHK_NO_FILL_AA
boolean DSM_Fill_AA = 0;
#else
boolean DSM_Fill_AA = 1;
#endif /* FEATURE_DSM_MEM_CHK_NO_FILL_AA */
#endif /* FEATURE_DSM_MEM_CHK */

#if DSM_TRACE_LEN > 0
#define DSM_LINE_NUMBER_SZ 8
char dsm_line_number_string[DSM_LINE_NUMBER_SZ];
#endif /* DSM_TRACE_LEN > 0 */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DSMI_VERIFY_PACKET()

DESCRIPTION 
  This will verify the correctness of a packet.  

  Check that a packet isn't linked to itself.  The algorythm is as
  follows.  Start 2 pointers at the beggining of the list (lead
  pointer and chase pointer).  On every iteration through the loop
  move the lead pointer down the list by one node, verify that it
  isn't pointing to the chase pointer.  On every other iteration
  through the loop move the chase pointer down the list.  If the
  packet is linked to itself, eventually these will meet.  If the
  packet is not linked to itself, eventually the lead pointer will get
  to the end of the packet.

DEPENDENCIES 
  item_ptr should be non-null and pointing at a dsm item allocated
  from dsm.
  
PARAMETERS
  item_ptr - Pointer to packet to verify

RETURN VALUE
  None (This function won't return if the check fails)

SIDE EFFECTS
  If the item_ptr fails a check then ERR_FATAL is called, otherwise none.
===========================================================================*/
void dsmi_verify_packet
(
  dsm_item_type *item_ptr
)
{
  uint32 cnt = 0;
  dsm_item_type *lead_ptr = item_ptr;
  dsm_item_type *chase_ptr = item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSM_ASSERT(item_ptr != NULL);

  while(lead_ptr != NULL)
  {
    /*-----------------------------------------------------------------
      Count buffers in packet 
    -----------------------------------------------------------------*/
    cnt ++;

    dsmi_verify_buffer(lead_ptr);
    /*-----------------------------------------------------------------
      Check for packet looped to self 
    -----------------------------------------------------------------*/
    lead_ptr = lead_ptr->pkt_ptr;
    if(lead_ptr == chase_ptr)
    {
      ERR_FATAL("dsm_verify_packet: Packet Linked to Self",0,0,0);
    }
    if((cnt & 0x1) == 0)
    {
      chase_ptr = chase_ptr->pkt_ptr;
    }
  }
}

/*===========================================================================
FUNCTION DSMI_VERIFY_BUFFER()

DESCRIPTION 
  This will verify the correctness of a buffer.

  - Verify that the head and tail cookies are ok.
  - Verify that the data_ptr[size] is the expected end of the buffer. 
  - Verify that used is less than or equal to size. 

DEPENDENCIES 
  item_ptr should be non-null and pointing at a dsm item allocated
  from dsm.
  
PARAMETERS
  item_ptr - Pointer to item to verify

RETURN VALUE
  None (This function won't return if the check fails)

SIDE EFFECTS
  If the item_ptr fails a check then ERR_FATAL is called, otherwise none.
===========================================================================*/
void dsmi_verify_buffer
(
  dsm_item_type *item_ptr
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DSM_MEM_CHK
  /*-----------------------------------------------------------------
    Check the head cookie is OK.
    -----------------------------------------------------------------*/
  if(*(uint32 *)(void *)((DSMI_ITEM_HEAD(item_ptr) - DSM_HEAD_COOKIE_LEN))
     != DSM_COOKIE
    )
  {
    ERR_FATAL("dsm_verify_buffer: Bad Header Cookie for item 0x%x",
              (int)item_ptr, 0, 0);
  }

  /*-----------------------------------------------------------------
    Check that the tail cookie is OK.
    -----------------------------------------------------------------*/
  if(*(uint32*)(void *)DSMI_ITEM_TAIL(item_ptr) != DSM_TAIL_COOKIE)
  {
    ERR_FATAL("dsm_verify_buffer: Bad Trailer Cookie for item 0x%x",
              (int)item_ptr, 0, 0);
  }

#endif /* FEATURE_DSM_MEM_CHK */
  
  /*-----------------------------------------------------------------
    Check data_ptr range.

    Check that used ok.  (common errors appear to be used gets
    decremented to less than 0 or size isn't being updated
    correctly.

    Check that non-dup does not have dup_ptr set

    If there's a size field, and strict checking, check size.
    -----------------------------------------------------------------*/
  if(!DSMI_DUP(item_ptr))
  {
    if( item_ptr->data_ptr < DSMI_ITEM_HEAD(item_ptr) ||
        item_ptr->data_ptr > DSMI_ITEM_TAIL(item_ptr)
        )
    {
      ERR_FATAL("dsm_verify_buffer: DATA_PTR 0x%x out of range for item 0x%x",
                (int)item_ptr->data_ptr,(int)item_ptr,0
                );

    }

    if( item_ptr->used >
        ( DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr )
      )
    {     
      ERR_FATAL("dsm_verify_buffer: USED field out of range %d:MAX %d item 0x%x",
                item_ptr->used,
                (int)(DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr),
                (int)item_ptr);
    }

/*     if( item_ptr->dup_ptr != NULL ) */
/*     { */
/*       ERR_FATAL("dsm_verify_buffer: DUP pointer set on non-dup", 0, 0, 0); */
/*     } */

#if defined(DSM_SIZE) && defined(FEATURE_DSM_MEM_CHK_STRICT)
    if(item_ptr->size != ( DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr ))
    {     
      ERR_FATAL("dsm_verify_buffer: SIZE field incorrect: %d:shd be %d item 0x%x",
                item_ptr->size,
                (int)( DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr ),
                (int)item_ptr);
    }
#endif /* FEATURE_DSM_MEM_CHK_STRICT && DSM_SIZE */

  } /* if not a DUP */

} /* dsmi_verify_buffer */

/*===========================================================================
FUNCTION DSMI_VERIFY_POOL_ID()

DESCRIPTION 
  This function verifies that the pool ID handed in is a valid one.
  (ie. in the set of pools we are currently managing.

DEPENDENCIES
  None
  
PARAMETERS
  pool_id - Pool id to check range of. 

RETURN VALUE
  None (This function won't return in pool_id is bogus.)

SIDE EFFECTS
  If the pool_id is bogus ERR_FATAL is called, otherwise none.
===========================================================================*/
void dsmi_verify_pool_id
(
  dsm_mempool_id_type pool_id 
)
{
  dsm_pool_mgmt_table_type *table=(dsm_pool_mgmt_table_type*)pool_id;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSM_ASSERT(table!=NULL);
  DSM_ASSERT((unsigned int)table > 256);

#ifdef FEATURE_DSM_MEM_CHK
  if(table->cookie != DSM_POOL_MGMT_TABLE_COOKIE)
  {
    ERR_FATAL("dsm: Invalid Pool ID = %d", (int)pool_id, 0, 0);
  }
#endif /* FEATURE_DSM_MEM_CHK */
}
/*===========================================================================
FUNCTION DSMI_VERIFY_MEM_LEVEL()

DESCRIPTION 
  This function verifies that the memory level handed in is a valid one.

DEPENDENCIES
  None
  
PARAMETERS
  mem_level - Memory level to check range of. 

RETURN VALUE
  None (This function won't return if mem_level is bogus.)

SIDE EFFECTS
  If the mem_level is bogus ERR_FATAL is called, otherwise none.
===========================================================================*/
void dsmi_verify_mem_level
(
  dsm_mem_level_enum_type mem_level 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!(mem_level > DSM_MEM_LEVEL_MIN && mem_level < DSM_MEM_LEVEL_MAX))
  {
    ERR_FATAL("dsm: Invalid Mem Level = %d", (int)mem_level, 0, 0);
  }
}

/*===========================================================================
FUNCTION DSMI_VERIFY_MEM_OP()

DESCRIPTION 
  This function verifies that the memory operation handed in is a valid one.

DEPENDENCIES
  None
  
PARAMETERS
  mem_op - Memory operation to check range of. 

RETURN VALUE
  None (This function won't return if mem_op is bogus.)

SIDE EFFECTS
  If the mem_op is bogus ERR_FATAL is called, otherwise none.
===========================================================================*/
void dsmi_verify_mem_op
(
  dsm_mem_op_enum_type mem_op 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!(mem_op > DSM_MEM_OP_MIN && mem_op < DSM_MEM_OP_MAX))
  {
    ERR_FATAL("dsm: Invalid Mem Operation = %d", (int)mem_op, 0, 0);
  }
}



/*===========================================================================
FUNCTION DSMI_REF_INC()

DESCRIPTION 
  Increment the reference count on a dsm item.

DEPENDENCIES
  None
  
PARAMETERS
  item_ptr - Pointer to item to act on. 

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
uint8 dsmi_ref_inc(dsm_item_type *item_ptr)
{
  uint8 ret;
  DSM_ASSERT(item_ptr != NULL);
  DSMI_POOL_LOCK(item_ptr->pool_id);
#ifdef FEATURE_DSM_MEM_CHK
  if(item_ptr->references == 0)
  {
    ERR_FATAL("dsmi_ref_inc: Reference count is zero for item 0x%x",
              (int)item_ptr,0,0);
  }
#endif /* FEATURE_DSM_MEM_CHK */
  ret = ++item_ptr->references;
  DSMI_POOL_UNLOCK(item_ptr->pool_id);
  return ret;
}

/*===========================================================================
FUNCTION DSMI_REF_DEC()
  INTFREE();

DESCRIPTION 
  Deccrement the reference count on a dsm item.

DEPENDENCIES
  None
  
PARAMETERS
  item_ptr - Pointer to item to act on. 

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
uint8 dsmi_ref_dec(dsm_item_type *item_ptr)
{
  uint8 ret = (uint8)~0;
  
  DSM_ASSERT(item_ptr != NULL);
  DSMI_POOL_LOCK(item_ptr->pool_id);
  if(item_ptr->references == 0) 
  {
#ifdef FEATURE_DSM_MEM_CHK
#ifndef FEATURE_DSM_MEM_CHK_ALLOW_DOUBLE_FREE
    ERR_FATAL("dsmi_ref_dec: Reference count already zero for item 0x%x",
              (int)item_ptr,0,0);
#endif /* FEATURE_DSM_MEM_CHK_ALLOW_DOUBLE_FREE */
#endif /* FEATURE_DSM_MEM_CHK */
  }
  else
  {
    ret = --item_ptr->references;
  }
  DSMI_POOL_UNLOCK(item_ptr->pool_id);
  return ret;
}



/*===========================================================================
FUNCTION DSMI_CHECK_MEM_POOL()

DESCRIPTION            
  This function goes through the memory pool and makes sure that there are
  no corrupted items.

  This is a test function and is used by developers using a debugger.

DEPENDENCIES
  
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  ERR FATALs if there is a problem with any of the active DSM items.
===========================================================================*/
STATIC void dsmi_check_mem_pool
(
  dsm_mempool_id_type pool_id
)
{
  dsm_item_type             *item_ptr;      /* pointer for data items */
  dsm_pool_mgmt_table_type  *table;         /* Temp table pointer */
  uint16                     i;             /* index to go through dsm array */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* first verify if the pool_id is valid! */
  dsmi_verify_pool_id(pool_id);
    
  table = (dsm_pool_mgmt_table_type*)pool_id;

  for (i=0; i < table->item_count; i++)
  {
    item_ptr = (dsm_item_type *) (void *)
      &(table->item_array[i * DSMI_POOL_OBJECT_SIZE(table->pool_item_size,
                                                    table->pad_size)]);

    dsmi_verify_buffer(item_ptr);
  }
} /* dsmi_check_mem_pool() */


/*===========================================================================
FUNCTION DSMI_CHECK_MEM_ALL_POOLS()

DESCRIPTION            
  This function goes through all of the memory pool and makes sure that there are
  no corrupted items.

  This is a test function and is used by developers using a debugger.

DEPENDENCIES
  
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  ERR FATALs if there is a problem with any of the active DSM items.
===========================================================================*/
void dsmi_check_mem_all_pools
(
  void
)
{
  dsm_pool_mgmt_table_type ** table_ptr = &dsm_pool_head_ptr;
  while( *table_ptr != NULL )
  {
    dsmi_check_mem_pool( (dsm_mempool_id_type)(*table_ptr) );
    table_ptr = &(*table_ptr)->next;
  }
}
