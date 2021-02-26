/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ P O O L . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_reg_mempool_events()
    Getister the passed fucntion pointer to be later invoked when the
    specified memory events happen for the specified memory pool.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/memory/dsm/src/dsm_pool.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/25/11    rj     Implemented DSM Memory Level event handling optimization
10/07/10    ag     If pool is already initialized, dont do init again.
01/12/10    ag     DSM Pool Tracing changes.
09/17/09    ag     Fixed lint warnings.
04/27/09    ag     Replaced assert.h with amssassert.h
01/18/07    mjb    Added support for per-pool locking.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "dsm_pool.h"
#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_lock.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "amssassert.h"

#ifdef FEATURE_DSM_NATIVE_ASSERT
#define DSM_ASSERT( cond )      assert( cond )
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================
  MACRO DSMI_ADJUST_ITEM_SIZE
  Round up to nearest 32, unless it's too close to 64K, then
  round down instead.
===========================================================================*/

#define DSMI_ADJUST_ITEM_SIZE(item_size)                         \
  ( ( item_size < (uint16)(~31) ) ? ((item_size + 31) & (~31)) : \
    ((uint16)(~31))\
  )

/*===========================================================================

FUNCTION DSMI_INIT_POOL_DATA()

DESCRIPTION
 Internal function that fills in fields in a pool management table.

DEPENDENCIES
  pool->item_cnt has to be set.

PARAMETERS
  pool_id - The indentifier for the pool that should be initialized.
  desc    - The pool description
  file    - FILE of calling function. (ie, could be dsm_init.c)
  line    - LINE of calling function.

RETURN VALUE
  None

SIDE EFFECTS
  Pool table values get filled in.
===========================================================================*/

static void dsmi_init_pool_data
(
 dsm_pool_mgmt_table_type *table,
 char * desc,
 const char * file,
 uint32 line
)
{
  uint32 i;			   /* Array iterator */
  dsm_item_type *item_ptr;	   /* pointer to a dsm item  */

  /* Fill in the table meta data */
  table->cookie = DSM_POOL_MGMT_TABLE_COOKIE;
  table->desc = desc;
  table->file = file;
  table->line = line;

  /* Invalidate the level callback array */
  table->next_level = DSM_MEM_LEVEL_MIN;
  for (i = (int)DSM_MEM_LEVEL_DNE;i < (int)DSM_MEM_LEVEL_MAX; i++)
  {
    table->mem_event_cb_list[i].level = DSM_MEM_LEVEL_INVALID;
    table->mem_event_cb_list[i].avail_item_count = 0;
    table->mem_event_cb_list[i].mem_free_event_cb = NULL;
    table->mem_event_cb_list[i].mem_new_event_cb = NULL;
    /*------------------------------------------------------------------------- 
      Initializing opposite level to DSM_MEM_LEVEL_INVALID. 
      DSM_MEM_LEVEL_INVALID is the default value which indicates optimized 
      callback will not take place for this Memory level unless it gets changed
      to an appropriate valid level
    -------------------------------------------------------------------------*/
    table->mem_event_cb_list[i].opposite_level = DSM_MEM_LEVEL_INVALID;
    table->mem_event_cb_list[i].is_optimized_callback = FALSE;
  }

  /* Iterate over the table and fill in each item */
  for (i=0; i < table->item_count; i++)
  {
    item_ptr =
      (dsm_item_type *)(void *)
      &(table->item_array[i * DSMI_POOL_OBJECT_SIZE(table->pool_item_size,
                                                    table->pad_size)]);
    item_ptr->pool_id = (dsm_mempool_id_type)table;
    DSM_ITEM_RESET(item_ptr);
    item_ptr->references = 0;
#ifdef FEATURE_DSM_MEM_CHK
    memset(item_ptr->data_ptr, 0xAA, item_ptr->size);
    item_ptr->trace[0].file = NULL;
    item_ptr->trace[0].line = 0;
    *(uint32 *)(item_ptr + 1) = DSM_COOKIE;
    *(uint32 *)(void *)&(item_ptr->data_ptr[item_ptr->size]) = DSM_TAIL_COOKIE;
#endif /* FEATURE_DSM_MEM_CHK */
    table->free_stack[table->free_count++] = item_ptr;
  }

  /* Initialize the min/max free counts. */
  table->min_free_count_interval = table->free_count;
  table->max_free_count_interval = table->free_count;

  /* Insert into pool linked list */
  table->next = dsm_pool_head_ptr;
  dsm_pool_head_ptr = table;
} /* dsmi_init_pool_data */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DSM_INIT_POOL()

DESCRIPTION
  Initialize a single data pool.  

DEPENDENCIES
  None

PARAMETERS
  pool_id - The identifier for the pool that should be initialized. 
  item_array - A pointer to a block of memory to be used for this pool.
  item_array_size - The size of the block of memory passed in item_array
  item_size - The size of the items that this pool should provide
  meta_size - Metadata size requested. This can only be requested for
              DUP pools, where item_size is 0.

RETURN VALUE
  An integer indicating the NUMBER of items that are available in this pool 
  after the "item_array" block of memory is split up inte "item_size" chunks
  plus overhead. 

SIDE EFFECTS
  None
===========================================================================*/

int dsmi_init_pool_desc
(
 dsm_mempool_id_type pool_id,
 uint8 * item_array,
 uint32 item_array_size,
 uint16 item_size,
 uint16 meta_size,
 char * desc,
 const char * file,
 uint32 line
)
{
  dsm_pool_mgmt_table_type *table; /* Temp table pointer */

  DSM_ASSERT( item_size == 0 || meta_size == 0 );

  /* Grab a short cut to the table */
  table = (dsm_pool_mgmt_table_type*)pool_id;

  /* Check if the pool is already initialized. */
  if(table->cookie == DSM_POOL_MGMT_TABLE_COOKIE)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"DSM Pool %d already initialized",pool_id);
    return table->item_count;
  }

  memset(table,0,sizeof(dsm_pool_mgmt_table_type));

  /* Item size must be evenly divisable by 32, round up */
  table->pool_item_size = DSMI_ADJUST_ITEM_SIZE(item_size);

  /*-------------------------------------------------------------------------
    Meta Data size aka PAD needs to be such that T + PAD + Header + H
    is a multiple of 32. Figure out how much more is needed given the
    meta size, and just make it so.
  -------------------------------------------------------------------------*/
  
  table->pad_size = meta_size +
                    (( 32 - ( ( sizeof(dsm_item_type) +
                                DSM_HEAD_COOKIE_LEN   +
                                meta_size             +
                                DSM_TAIL_COOKIE_LEN ) & 31) ) & 31 ) ;
  
  /* Align the item_array so that item data lands on a 32 byte boundary */
  /*  
     
     +---------------+---------------+---------------+---------------+
     |T|  pad     | Header         |H|Data                           |T|
     +---------------+---------------+---------------+---------------+

       +     - indicate 32 byte boundaries.
       T     - is the tail cookie from the previous buffer.
       pad   - is padding so that things line up correctly.
       H     - is the head cookie for this buffer
       Data  - is the data for this buffer. data may be any multiple of
               32 bytes.
 
  */

  /* align table to even-32 + (cookie size + pad) */
  table->item_array =
    (uint8*)( ( ( (uint32)item_array + 31 ) & ~31 ))  + 
    DSM_TAIL_COOKIE_LEN                               +
    table->pad_size ;

  item_array_size -= table->item_array - item_array;

  /* Set the item count */ 
  table->item_count =
    (uint16) (item_array_size / 
              (DSMI_POOL_OBJECT_SIZE(table->pool_item_size,table->pad_size) +
               sizeof(void *)));
  table->stats_min_free_count = table->item_count;

  /* Figure out where the free stack starts */
  table->free_stack = (dsm_item_type**)(void*)
    &(table->item_array)
    [ ( table->item_count *
        DSMI_POOL_OBJECT_SIZE(table->pool_item_size,table->pad_size)) -
      table->pad_size
    ];

  dsmi_init_pool_data(table,desc,file,line);

  /* create a lock for the pool if needed */
  DSM_LOCK_CREATE( &(table->lock) );
  
  return table->item_count;
} /* dsmi_init_pool_desc */

/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_CB()

DESCRIPTION
  This function will register the passed function pointers to be later 
  invoked if the specified events occur for the specified memory pool.
  
  Example: One can register dsi_mem_event_ctrl() function of dsmgr as
  a callback to be invoked when the number of free items reaches 
  DSM_DS_SMALL_ITEM_FEW_MARK during memory allocations from small item
  pool as follows.

  dsm_reg_mem_event_cb
  (
    DSM_DS_SMALL_ITEM_POOL,
    DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW,
    dsi_mem_event_ctrl    
  );

  Note that the corresponding unreg() function has been defined as a macro
  in dsm.h file.

DEPENDENCIES
  None

PARAMETERS
  pool_id - Which memory pool to register with.
  level - The level associated with this callback. 
  mem_op - The memory operation (new/free) the callback is associated with.
  mem_event_cb - Pointer to function to be registered as a callback.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsm_reg_mem_event_cb
(
  dsm_mempool_id_type      pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_op_enum_type     mem_op,
  void                     (*mem_event_cb) (dsm_mempool_id_type,
                                            dsm_mem_level_enum_type,
                                            dsm_mem_op_enum_type)
)
{
  dsm_pool_mgmt_table_type *table;  /* temp table pointer */
  uint32 i;                     /* Array iterator */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Ensure that the Pool ID parameter is valid.
  -------------------------------------------------------------------------*/
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */

  /*-------------------------------------------------------------------------
    Verify that the level parameter is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_level(level);

  /*-------------------------------------------------------------------------
    Verify that the memory operation specified is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_op(mem_op);

#endif

  /*-------------------------------------------------------------------------
    The passed parameters have been validated. Make the entry for the
    specified callback.
  -------------------------------------------------------------------------*/
  table = (dsm_pool_mgmt_table_type*)pool_id;

  DSMI_POOL_LOCK(table);

  for(i = (int)DSM_MEM_LEVEL_MIN + 1; 
      i < (int)DSM_MEM_LEVEL_MAX && 
        table->mem_event_cb_list[i].level != level &&
        table->mem_event_cb_list[i].level != DSM_MEM_LEVEL_INVALID; 
      i++);

  /* If this assert fires someone is trying to register a mem level callback
     for a memory level that has not been configured. */
  if(  i >= (int)DSM_MEM_LEVEL_MAX ||
       table->mem_event_cb_list[i].level == DSM_MEM_LEVEL_INVALID
    )
  {
    ERR_FATAL
    (
      "Attempt to register a mem level CB for a non-configured mem level %d",
      (int)level,0,0
    );
  }
  else if(table->mem_event_cb_list[i].level == level)
  {
    if (mem_op == DSM_MEM_OP_FREE)
    {
      table->mem_event_cb_list[i].mem_free_event_cb = mem_event_cb;
    }
    else if (mem_op == DSM_MEM_OP_NEW)
    {
      table->mem_event_cb_list[i].mem_new_event_cb = mem_event_cb;
    }
  }

  DSMI_POOL_UNLOCK(table);

  return;
} /* dsm_reg_mem_event_cb */


/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_LEVEL()

DESCRIPTION
  This function will set the avail_item_count for a given memory level.  

DEPENDENCIES
  None

PARAMETERS
  pool_id - Which memory pool to register with.
  level - The level we want to register.. 
  avail_item_count - The count for this level

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsm_reg_mem_event_level
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type level,
  uint32 avail_item_count
)
{
  dsm_pool_mgmt_table_type *table; /* temp table pointer */
  uint32 i,j;			/* Array interator */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Ensure that the Pool ID parameter is valid.
  -------------------------------------------------------------------------*/
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */
#endif

  /*-------------------------------------------------------------------------
    Verify that the level parameter is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_level(level);

  /*-------------------------------------------------------------------------
    The passed parameters have been validated. Make the entry for the
    specified callback.
  -------------------------------------------------------------------------*/
  table = (dsm_pool_mgmt_table_type*)pool_id;

  DSMI_POOL_LOCK(table);
  /* Search for the specified level or an unused slot */
  for(i = (int)DSM_MEM_LEVEL_MIN + 1; i < (int)DSM_MEM_LEVEL_MAX; i++)
  { 
    if(table->mem_event_cb_list[i].level == level ||
       table->mem_event_cb_list[i].level == DSM_MEM_LEVEL_INVALID)
    {
      break;
    }
  }

  if( i >= (int)DSM_MEM_LEVEL_MAX )
  {
    ERR_FATAL("Unable to find unused slot in pool callback table",0,0,0);
  }

  /* 'i' should reference a slot we can use. Fill it in*/
  table->mem_event_cb_list[i].avail_item_count = 
    (uint16)avail_item_count;
  table->mem_event_cb_list[i].level = level;

  /* Resort the array (Yes this is bubble sort, yes its slow,
     this array has ~9 items in it, so I don't care */
  for(i = (int)DSM_MEM_LEVEL_MIN + 2; 
      i < (int)DSM_MEM_LEVEL_MAX &&
        table->mem_event_cb_list[i].level != DSM_MEM_LEVEL_INVALID; 
      i++)
  {
    for(j = (int)DSM_MEM_LEVEL_MIN + 2; 
        j <= (int)DSM_MEM_LEVEL_MAX - i &&
          table->mem_event_cb_list[j].level != DSM_MEM_LEVEL_INVALID; 
        j++)
    {
      if(table->mem_event_cb_list[j-1].avail_item_count > 
       table->mem_event_cb_list[j].avail_item_count)
      {
        dsm_mem_event_cb_entry tmp;
        tmp = table->mem_event_cb_list[j-1];
        table->mem_event_cb_list[j-1] = table->mem_event_cb_list[j];
        table->mem_event_cb_list[j] = tmp;
      }
    }
  }

  /* Figure out what next_level should be */
  for(i = (int)DSM_MEM_LEVEL_MIN + 1; 
      i < (int)DSM_MEM_LEVEL_MAX &&
        table->mem_event_cb_list[i].level != DSM_MEM_LEVEL_INVALID &&
        table->free_count >= table->mem_event_cb_list[i].avail_item_count; 
      i++);
  table->next_level = (dsm_mem_level_enum_type)(i - 1);

#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB  
  /*------------------------------------------------------------------------- 
    If a Memory level has been registered for an optimized callback, 
    opposite_level_index stores the index for it's corresponding opposite
    level. Usually, opposite_level_index is computed within 
    dsm_reg_mem_event_optimized_level. But in the scenario where a Memory 
    event level registration happens at any time after opposite_level_indexes
    have been computed for various other levels, we need to recompute the 
    opposite indexes for all those levels after the mem_event_cb_list 
    has been re-sorted.
  -------------------------------------------------------------------------*/
  for(i = (int)DSM_MEM_LEVEL_MIN + 1; 
      i < ((int)DSM_MEM_LEVEL_MAX - 1) && 
        table->mem_event_cb_list[i].level != DSM_MEM_LEVEL_INVALID; i++) 
  {
    if(table->mem_event_cb_list[i].opposite_level != DSM_MEM_LEVEL_INVALID) 
    {
      for(j = i+1; 
          j < (int)DSM_MEM_LEVEL_MAX && 
            table->mem_event_cb_list[j].level != DSM_MEM_LEVEL_INVALID; j++)
      {
        if(table->mem_event_cb_list[j].opposite_level == 
            table->mem_event_cb_list[i].level)
        {
          table->mem_event_cb_list[i].opposite_level_index = (uint16)j;
          table->mem_event_cb_list[j].opposite_level_index = (uint16)i;
          break;
        }
      }
    }
  }
#endif /*FEATURE_DSM_POOL_OPTIMIZED_CB*/  

  DSMI_POOL_UNLOCK(table);

  return;
} /* dsm_reg_mem_event_level */

/*===========================================================================
FUNCTION DSM_POOL_MEM_EVENT_LEVEL()

DESCRIPTION
  This function returns the memory level in bytes for an enum level for
  a memory pool.

DEPENDENCIES
  None

PARAMETERS
  pool_id - Which memory pool to search through.
  level   - The level we want to index

RETURN VALUE
  Memory level in bytes for the given level_type

SIDE EFFECTS
  None
===========================================================================*/

uint32 dsm_pool_mem_event_level
(
 dsm_mempool_id_type pool_id,
 dsm_mem_level_enum_type level
)
{
  dsm_pool_mgmt_table_type *table; /* temp table pointer */
  uint32 i;			/* Array interator */
  uint32 ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Ensure that the Pool ID parameter is valid.
  -------------------------------------------------------------------------*/
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */
#endif

  /*-------------------------------------------------------------------------
    Verify that the level parameter is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_level(level);

  /*-------------------------------------------------------------------------
    The passed parameters have been validated. Make the entry for the
    specified callback.
  -------------------------------------------------------------------------*/
  table = (dsm_pool_mgmt_table_type*)pool_id;

  DSMI_POOL_LOCK(table);
  /* Search for the specified level or an unused slot */
  for(i = (int)DSM_MEM_LEVEL_MIN + 1; i < (int)DSM_MEM_LEVEL_MAX; i++)
  { 
    if(table->mem_event_cb_list[i].level == level ||
       table->mem_event_cb_list[i].level == DSM_MEM_LEVEL_INVALID)
    {
      break;
    }
  }

  if( i >= (int)DSM_MEM_LEVEL_MAX )
  {
    ERR_FATAL("Unable to find mem level in pool callback table",0,0,0);
  }

  /* If this assert fires someone is trying to register a mem level callback
     for a memory level that has not been configured. */
  DSM_ASSERT(table->mem_event_cb_list[i].level == level);
  ret = table->mem_event_cb_list[i].avail_item_count;
  DSMI_POOL_UNLOCK(table);
  return ret;
} /* dsm_pool_mem_event_level */

/*===========================================================================
FUNCTION DSM_REMOVE_POOL()

DESCRIPTION
  Removes a DSM pool from the linked list and resets cookie and rest of
  pool management table content. It does NOT free any DSM memory itself.
  It's up to the user to free up the buffer memory.

DEPENDENCIES
  None

PARAMETERS
  pool_id - Which memory pool to remove.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory pool is removed from linked list.
===========================================================================*/

void dsm_remove_pool
(
 dsm_mempool_id_type pool_id
)
{
  dsm_pool_mgmt_table_type *table; /* Temp table pointer */

  dsm_pool_mgmt_table_type **t_ptr;
  
  /* Grab a short cut to the table */
  table = (dsm_pool_mgmt_table_type*)pool_id;
  DSM_ASSERT(table->cookie == DSM_POOL_MGMT_TABLE_COOKIE);

  if(table->item_count != table->free_count)
  {
    ERR_FATAL("Memory pool has %d unfreed items still in use",
              table->item_count - table->free_count,0,0);
  }

  t_ptr = &dsm_pool_head_ptr;

  while( *t_ptr != NULL && *t_ptr != table )
  {
    t_ptr = &(*t_ptr)->next;
  }
  
  if( *t_ptr == NULL )
  {
    ERR_FATAL("Memory pool could not be found in list of pools",0,0,0);
  }

  *t_ptr = (*t_ptr)->next;

  /* remove locking semaphore */
  DSM_LOCK_DESTROY( &(table->lock) );

  memset(table,0,sizeof(dsm_pool_mgmt_table_type));

} /* dsm_remove_pool */

/*===========================================================================
FUNCTION DSM_POOL_REG_MEM_FREE_EVENT_CB

DESCRIPTION
  Registers a callback that is called just before a dsm item is freed
  and returned to the stack.

DEPENDENCIES
  None

PARAMETERS
  pool_id               - Which pool to assign callback.
  free_event_user_data  - User defined handle.
  free_event_cb         - Callback to call.

RETURN VALUE
  None.

SIDE EFFECTS
  Callback is registered in pool management table, which will be called.
===========================================================================*/

void dsm_pool_reg_mem_free_event_cb
(
  dsm_mempool_id_type pool_id,
  void (*free_event_cb)
  (
     void * user_data,
     dsm_item_type * item
  ),
  void * free_event_user_data
)
{
  dsm_pool_mgmt_table_type *table; /* Temp table pointer */
  table = (dsm_pool_mgmt_table_type *)pool_id;
  table->free_cb_user_data = free_event_user_data;
  table->free_cb = free_event_cb;
} /* dsm_pool_reg_mem_free_event_cb */

/*===========================================================================
FUNCTION DSM_GET_POOL_STATS

DESCRIPTION
  Returns all the min/max/current free count and total item count for a
  DSM pool. It also returns the available item counts for the free, many and
  dne memory levels for the particular dsm pool.

DEPENDENCIES
  If FEATURE_DSM_POOL_TRACING is not defined, this function will 
  ERR_FATAL().

PARAMETERS
  pool_id               - Which pool to assign callback.
  dsm_pool_cnts         - Pointer to a dsm_pool_counts_type structure.

RETURN VALUE
  None.

SIDE EFFECTS
  The pool is locked while retreiving all the pool counts.
===========================================================================*/
void dsm_get_pool_stats
(
  /* Pool id */
  dsm_mempool_id_type pool_id,

  /* Pointer to a dsm_pool_counts_type structure. */
  dsm_pool_counts_type* dsm_pool_cnts
)
{
#ifdef FEATURE_DSM_POOL_TRACING
  dsm_pool_mgmt_table_type *table; 
  
  /* Ensure that the Pool ID parameter is valid. */
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */

  /* Ensure that the pointer is not NULL */
  DSM_ASSERT(dsm_pool_cnts != NULL);

  /* Memset the dsm_pool_cnts to 0*/
  memset(dsm_pool_cnts, 0, sizeof(dsm_pool_counts_type));

  /* Get the table pointer to the pool structure */
  table = (dsm_pool_mgmt_table_type*)pool_id;

  /* Lock the pool mutex */
  DSMI_POOL_LOCK(table);

  /* Copy Pool id */
  dsm_pool_cnts->pool_id = pool_id;

  /* Copy total item count  */
  dsm_pool_cnts->total_item_count = table->item_count;

  /* Copy current free count */
  dsm_pool_cnts->current_free_count = table->free_count;

  /* Copy min free count for the interval */
  dsm_pool_cnts->min_free_count = table->min_free_count_interval;

  /* Copy max free count for the interval */
  dsm_pool_cnts->max_free_count = table->max_free_count_interval;

  /* Reset the min and max free counts for the next interval */
  table->min_free_count_interval = table->free_count;
  table->max_free_count_interval = table->free_count;

  /* Unlock the pool mutex */
  DSMI_POOL_UNLOCK(table); 
#else
  ERR_FATAL("FEATURE_DSM_POOL_TRACING is not defined.", 0,0,0);
#endif /* FEATURE_DSM_POOL_TRACING */

} /* dsm_get_pool_stats */


/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_OPTIMIZED_LEVEL()

DESCRIPTION
  This function will register both the level as well as its opposite level
  passed to this function by the user. An example would be to register 
  DSM_MEM_LEVEL_SIO_MANY and DSM_MEM_LEVEL_SIO_FEW as a pair.

DEPENDENCIES
  Only 1 unique level/opposite_level pair can co-exist. For example, if 
  DSM_MEM_LEVEL_SIO_MANY has already been registered for an optimized
  callback with DSM_MEM_LEVEL_SIO_FEW, it cannot exist as a pair 
  with any other level. 

PARAMETERS
  pool_id - Which memory pool to register with.
  level - The level associated with this callback. 
  opposite_level - The opposite level associated with this level

RETURN VALUE
  None

SIDE EFFECTS
  The pool is locked while registering level/opposite_level pair
===========================================================================*/
void dsm_reg_mem_event_optimized_level
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_level_enum_type  opposite_level
)
{
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
  dsm_pool_mgmt_table_type *table; 
  uint32 i;
  int level_index, opposite_level_index; 

  level_index = (int)DSM_MEM_LEVEL_INVALID;
  opposite_level_index = (int)DSM_MEM_LEVEL_INVALID;

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Ensure that the Pool ID parameter is valid.
  -------------------------------------------------------------------------*/
  dsmi_verify_pool_id(pool_id); 
#endif

  /*-------------------------------------------------------------------------
    Verify that the level parameter is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_level(level);

  /*-------------------------------------------------------------------------
    Verify that the opposite_level parameter is valid
  -------------------------------------------------------------------------*/
  dsmi_verify_mem_level(opposite_level);

  /* Get the table pointer to the pool structure */
  table = (dsm_pool_mgmt_table_type*)pool_id;

  DSMI_POOL_LOCK(table);

  /* Check whether level or opposite_level has already been registered     */
  for(i = (int)DSM_MEM_LEVEL_MIN + 1; 
      i < (int)DSM_MEM_LEVEL_MAX && 
       table->mem_event_cb_list[i].level != DSM_MEM_LEVEL_INVALID; i++)
  { 
    if(table->mem_event_cb_list[i].opposite_level == level)
    {
      ERR_FATAL("Level %x has already been registered at index %d",
                (int)level,i,0);
    }
    if(table->mem_event_cb_list[i].opposite_level == opposite_level)
    {
      ERR_FATAL("Opposite Level %x has already been registered at index %d",
                (int)opposite_level,i,0);
    }
    if(table->mem_event_cb_list[i].level == level)
    {
      level_index = i;
    }
    if(table->mem_event_cb_list[i].level == opposite_level)
    {
      opposite_level_index = i;
    }
  }

  if(level_index == (int)DSM_MEM_LEVEL_INVALID ||
     opposite_level_index == (int)DSM_MEM_LEVEL_INVALID)
  {
    ERR_FATAL
    (
      "Attempt to register optimized CB for one or both non-configured mem levels %d %d",
      (int)level,(int)opposite_level,0
    );
  }

  /*------------------------------------------------------------------------
    Register level and its corresponding opposite level                                  
  -------------------------------------------------------------------------*/

  table->mem_event_cb_list[level_index].opposite_level = opposite_level;
  table->mem_event_cb_list[level_index].is_optimized_callback =  TRUE;
  table->mem_event_cb_list[level_index].opposite_level_index = 
    (uint16)opposite_level_index;
  
  table->mem_event_cb_list[opposite_level_index].opposite_level = level;
  table->mem_event_cb_list[opposite_level_index].is_optimized_callback = TRUE;
  table->mem_event_cb_list[opposite_level_index].opposite_level_index =
    (uint16)level_index;
  

  DSMI_POOL_UNLOCK(table);
  return; 
#else 
  DSM_NOTUSED(pool_id);
  DSM_NOTUSED(level);
  DSM_NOTUSED(opposite_level);
#endif /*FEATURE_DSM_POOL_OPTIMIZED_CB*/
}

/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_OPTIMIZED_LEVEL_COMBINED()

DESCRIPTION
  This is a combined API which calls dsm_reg_mem_event_level for both levels
  pair and then calls dsm_reg_mem_event_optimized_level to enable optimized
  callback

DEPENDENCIES
  The assumption is that if the user chooses to use this combined API, both
  the levels should be registered using this API only.Hence there is no need
  to register both those levels seperately using dsm_reg_mem_event_level.

PARAMETERS
  pool_id - Which memory pool to register with.
  level - The level associated with this callback.
  avail_item_count_level - The count for this level
  opposite_level - The opposite level associated with this level
  avail_item_count_opposite_level - The count for the opposite level

RETURN VALUE
  None

SIDE EFFECTS
The pool is locked when the above mentioned API's are called
===========================================================================*/
void dsm_reg_mem_event_optimized_level_combined
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type level,
  uint32 avail_item_count_level,
  dsm_mem_level_enum_type opposite_level,
  uint32 avail_item_count_opposite_level
)
{
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
  /* Register level and its count                */
  dsm_reg_mem_event_level(pool_id,
                          level,
                          avail_item_count_level);

  /* Register Opposite level and its count       */
  dsm_reg_mem_event_level(pool_id,
                          opposite_level,
                          avail_item_count_opposite_level);

  /* Register both levels for optimized callback */
  dsm_reg_mem_event_optimized_level(pool_id,
                                    level,
                                    opposite_level);
#else 
  DSM_NOTUSED(pool_id);
  DSM_NOTUSED(level);
  DSM_NOTUSED(avail_item_count_level);
  DSM_NOTUSED(opposite_level);
  DSM_NOTUSED(avail_item_count_opposite_level);
#endif /*FEATURE_DSM_POOL_OPTIMIZED_CB*/
}
