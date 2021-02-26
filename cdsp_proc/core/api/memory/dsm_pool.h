#ifndef DSM_POOL_H
#define DSM_POOL_H
/*===========================================================================

                                  D S M _ P O O L . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1.c4/api/memory/dsm_pool.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/25/11    rj     Implemented DSM Memory Level event handling optimization
04/21/11    rj     Remove Feature DSM_ALIGNED_ALLOC. 
10/05/10    sb     Added memory level enums for WLAN. 
09/29/10    ag     Removed DSM_WINCE and DSM_NATIVE_LINUX features. 
06/15/10    sb     Add new LTE specific enums in dsm_mem_level_enum_type 
01/13/10    ag     Removed featurization for CMI Phase 2. 
01/12/10    ag     DSM Pool Tracing changes.
08/03/09    ag     Added memory levels for HSUSB & SMD.
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                        MACRO DECLARATIONS
===========================================================================*/


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Predeclaration of dsm_item_type.
---------------------------------------------------------------------------*/

struct dsm_item_s; 

typedef struct dsm_item_s dsm_item_type;
typedef int dsm_mempool_id_type;

/*  This is not an ENUM. It is called this for historical reasons. */
typedef int dsm_mempool_id_enum_type;


/*---------------------------------------------------------------------------
  Types of memory operations, used to associate a callback with a particular
  operation.
---------------------------------------------------------------------------*/

typedef enum
{
  DSM_MEM_OP_MIN     = -1,
  DSM_MEM_OP_NEW     =  0,
  DSM_MEM_OP_FREE    =  1,
  DSM_MEM_OP_MAX     =  2
} dsm_mem_op_enum_type;

/*---------------------------------------------------------------------------
  Callbacks can be registered for the following levels. One can think of a
  level as an index which lets DSM know where to insert the callback
  entry. The free_count associated with these levels should be in 
  non-descending order, i.e., free_count for FEW level >= free_count for DNE 
  level and so on. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSM_MEM_LEVEL_INVALID         = -2, 
  DSM_MEM_LEVEL_MIN             = -1,
  DSM_MEM_LEVEL_DNE             = 0,
  DSM_MEM_LEVEL_LINK_LAYER_DNE  = 1,
  DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW = 2,
  DSM_MEM_LEVEL_LINK_LAYER_FEW  = 3,
  DSM_MEM_LEVEL_HDR_FEW         = 4, 
  DSM_MEM_LEVEL_RLC_FEW         = 5,
  DSM_MEM_LEVEL_FEW             = 6,
  DSM_MEM_LEVEL_SIO_FEW         = 7,
  DSM_MEM_LEVEL_HDR_MANY        = 8,
  DSM_MEM_LEVEL_RLC_MANY        = 9,
  DSM_MEM_LEVEL_MANY            = 10,
  DSM_MEM_LEVEL_SIO_MANY        = 11,
  DSM_MEM_LEVEL_SMD_FEW         = 12,
  DSM_MEM_LEVEL_SMD_MANY        = 13,
  DSM_MEM_LEVEL_USB_FEW         = 14,
  DSM_MEM_LEVEL_USB_MANY        = 15,
  DSM_MEM_LEVEL_LTE_DNE         = 16,   
  DSM_MEM_LEVEL_LTE_DL_FEW      = 17,
  DSM_MEM_LEVEL_LTE_DL_MANY     = 18,
  DSM_MEM_LEVEL_A2_UL_PER_FEW   = 19,
  DSM_MEM_LEVEL_A2_UL_PER_MANY  = 20,
  DSM_MEM_LEVEL_WLAN_FEW        = 21,
  DSM_MEM_LEVEL_WLAN_MANY       = 22,
  DSM_MEM_LEVEL_MAX 
} dsm_mem_level_enum_type;

/*---------------------------------------------------------------------------
  DSM uses the following struct to store information about memory event
  callbacks. A maximum of two callbacks can be associated at a particular
  level, one for free operation and the other for new. If more callbacks
  need to be registered for the same free count, one needs to add another
  level. 
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_mem_level_enum_type level;
  uint16 avail_item_count;        /* cb is called when free_count equals   */
                                  /* this count                            */

  dsm_mem_level_enum_type opposite_level;
                                  /* Opposite Level enum value for this 
                                     level                                 */  
  boolean is_optimized_callback;  /* Boolean to determine whether to do an 
                                     optimized callback or not             */ 
  uint16  opposite_level_index;   /* Index location for the opposite level */  

  void   (*mem_free_event_cb) (dsm_mempool_id_type, 
                               dsm_mem_level_enum_type,
                               dsm_mem_op_enum_type);
                                  /* the cb associated with free operation */
  void   (*mem_new_event_cb)  (dsm_mempool_id_type, 
                               dsm_mem_level_enum_type,
                               dsm_mem_op_enum_type);
                                  /* the cb associated with new operation  */
} dsm_mem_event_cb_entry;



/*---------------------------------------------------------------------------
  Type for the Table definition for managing the memory free pools.

  free_count: The current number of available memory pool items.

  next_level: The next level for which the next callback is to be called.
  We follow the convention that next_level points to the level in the
  direction of lower memory. For example, if level 1 corresponds to 
  free count of 5, level 2 to 10 and current free_count is 8 then next_level
  would be 1. In other words, we call the cb at next_level for new operations
  and at next_level+1 for free operations.

  mem_event_cb_list: The list of callbacks to be invoked when a particular
  memory event occurs, i.e., free_count drops to or reaches a particular 
  level.
---------------------------------------------------------------------------*/
typedef struct dsm_pool_mgmt_table_s
{
  int cookie;                   /* Magic Cookie                            */ 
  uint8 *item_array;		/* Pointer to Memory Array                 */
  dsm_item_type **free_stack;   /* Pointer to the free stack               */
  uint16  free_count;		/* The current number of items in Free     */
                                /* Queue                                   */
  uint16  item_count;		/* The max number of items in the pool     */
  uint16  pool_item_size;	/* Size of items for the pool              */
  uint16  pad_size;             /* Size of pad,also size of metadata       */
  
  uint16  stats_min_free_count;	/* Minimum count the free pool dropped to  */

  uint16 min_free_count_interval; /* Minumum value of free count in an 
                                     interval */

  uint16 max_free_count_interval; /* Maximum value of free count in an 
                                     interval*/

  dsm_mem_level_enum_type  next_level; 
  dsm_mem_event_cb_entry mem_event_cb_list[DSM_MEM_LEVEL_MAX];

  /* free callback */
  void (*free_cb) (void * user_data,dsm_item_type * item);
  void * free_cb_user_data;

  struct dsm_pool_mgmt_table_s * next;   /* pointer to next pool */
  char * desc;

  const char * file;
  uint32 line;

  dword  lock;   /* lock for pool */
  
} dsm_pool_mgmt_table_type;


/* Structure used to return results for DSM Pool Tracing. */
typedef struct dsm_pool_counts_s
{
  /* Pool Id */
  dsm_mempool_id_type pool_id;

  /* Total number of items in the pool */
  uint16 total_item_count;

  /* Number of items currently free in the pool */
  uint16 current_free_count;

  /* Minimum value of free_count in this interval */
  uint16 min_free_count;

  /* Maximum value of free_count in this interval */
  uint16 max_free_count;

}dsm_pool_counts_type;

/*---------------------------------------------------------------------------
  Memory level macros.... 
---------------------------------------------------------------------------*/
#define DSM_MORE_THAN_MANY_FREE_ITEMS(pool_id)                              \
 (DSM_POOL_FREE_CNT(pool_id) >=                                             \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_MANY))

#define DSM_LESS_THAN_MANY_FREE_ITEMS(pool_id)                              \
 (DSM_POOL_FREE_CNT(pool_id) <                                              \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_MANY))

#define DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id)                               \
 (DSM_POOL_FREE_CNT(pool_id) <                                              \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_FEW))

#define DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS(pool_id)                    \
 (DSM_POOL_FREE_CNT(pool_id) <                                              \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_LINK_LAYER_FEW))

#define DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(pool_id)               \
 (DSM_POOL_FREE_CNT(pool_id) <                                              \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW))

#define DSM_DONT_EXCEED_ITEMS(pool_id)                                      \
 (DSM_POOL_FREE_CNT(pool_id) <=                                             \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_DNE))

#define DSM_LINK_LAYER_DONT_EXCEED_ITEMS(pool_id)                           \
 (DSM_POOL_FREE_CNT(pool_id) <                                              \
    DSM_POOL_MEM_EVENT_LEVEL(pool_id,DSM_MEM_LEVEL_LINK_LAYER_DNE))

/*---------------------------------------------------------------------------
  Macro to return the size of items in a given dsm pool.
---------------------------------------------------------------------------*/
#define DSM_POOL_ITEM_SIZE(pool_id)                                         \
  (((dsm_pool_mgmt_table_type*)pool_id)->pool_item_size)

/*---------------------------------------------------------------------------
  This MACRO will return free count for the givin pool.
---------------------------------------------------------------------------*/
#define DSM_POOL_FREE_CNT(pool_id)                                          \
  (((dsm_pool_mgmt_table_type*)pool_id)->free_count)

/*---------------------------------------------------------------------------
  This MACRO will return item count for the given pool.
---------------------------------------------------------------------------*/
#define DSM_POOL_ITEM_CNT(pool_id)                                          \
  (((dsm_pool_mgmt_table_type*)pool_id)->item_count)


/*---------------------------------------------------------------------------
  This Macro will return the level for a given memory pool and memory level. 
  This level is the absolute count of number of dsm items.
---------------------------------------------------------------------------*/
#define DSM_POOL_MEM_EVENT_LEVEL(pool_id,lvl)                               \
  dsm_pool_mem_event_level(pool_id,lvl)

/*---------------------------------------------------------------------------
  This MACRO will return the pad size for the given pool.
  This works for ANY pool, ANY pool can have padding for alignment, but
  only DUP pools allow meta data.
---------------------------------------------------------------------------*/
#define DSM_POOL_PAD_SIZE(pool_id)                                          \
  (((dsm_pool_mgmt_table_type*)pool_id)->pad_size)
    
/* External Data Declarations */
extern dsm_pool_mgmt_table_type * dsm_pool_head_ptr;
				     
/*===========================================================================
MACRO DSM_UNREG_MEM_EVENT_CB()

DESCRIPTION
  This macro will call dsm_reg_mem_event_cb() to unregister the callback 
  registered at the specified level for the given memory operation in the 
  specified memory pool.

  Example: One can unregister the new callback for the FEW level for small
  item pool as follows.

  dsm_unreg_mem_event_cb( DSM_DS_SMALL_ITEM_POOL, 
                          DSM_MEM_LEVEL_FEW,
                          DSM_MEM_OP_NEW );

DEPENDENCIES
  None

PARAMETERS
  pool_id - Which memory pool to unregister from.
  level - The level for which the the callback is to be unregistered.
  mem_op - The memory operation (free/new) for which the callback is to 
  be unregistered.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_unreg_mem_event_cb(pool_id, level, mem_op)                      \
  dsm_reg_mem_event_cb(pool_id, level, mem_op, NULL);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/*===========================================================================
                      FUNCTION DECLARATIONS

===========================================================================*/
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

RETURN VALUE
  An integer indicating the NUMBER of items that are available in this pool 
  after the "item_array" block of memory is split up inte "item_size" chunks
  plus overhead. 

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_init_pool(pool_id,item_array,item_array_size,item_size)   \
  dsmi_init_pool_desc(pool_id,item_array,item_array_size,item_size,   \
                      0,NULL,__FILE__,__LINE__)

#define dsm_init_pool_desc(pool_id,item_array,item_array_size,        \
                           item_size,desc)                            \
  dsmi_init_pool_desc(pool_id,item_array,item_array_size,item_size,   \
                      0,desc,__FILE__,__LINE__)

#define dsm_init_pool_meta(pool_id,item_array,item_array_size,          \
                           meta_size,desc)                              \
  dsmi_init_pool_desc(pool_id,item_array,item_array_size,               \
                      0,meta_size,desc,__FILE__,__LINE__)


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
);


/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_CB()

DESCRIPTION
  This function will register the passed function pointer to be later 
  invoked if the specified event occurs for the specified memory pool.
  
  Example: One can register dsi_mem_event_ctrl() function of dsmgr as
  a callback to be invoked when the memory level drops to DSM_MEM_LEVEL_FEW
  during memory allocations from small item pool as follows. The free count
  associated with DSM_MEM_LEVEL_FEW is assigned as part of initialization 
  and it cannot be changed dynamically. 

  dsm_reg_mem_event_cb
  (
    DSM_DS_SMALL_ITEM_POOL,
    DSM_MEM_LEVEL_FEW,
    DSM_MEM_OP_NEW,
    dsi_mem_event_ctrl    
  );

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
extern void dsm_reg_mem_event_cb
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_op_enum_type     mem_op,
  void                     (*mem_event_cb) (dsm_mempool_id_type,
                                            dsm_mem_level_enum_type,
                                            dsm_mem_op_enum_type) 
);

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

extern void dsm_reg_mem_event_level
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type level,
  uint32 avail_item_count
);

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
extern uint32 dsm_pool_mem_event_level
(
 dsm_mempool_id_type pool_id,
 dsm_mem_level_enum_type level
);

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
);

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
  Callback is registered in pool management table.
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
);  
  
/*===========================================================================
FUNCTION DSM_GET_POOL_STATS

DESCRIPTION
  Returns all the min/max/current free count and total item count for a
  DSM pool. 

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
);


/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_OPTIMIZED_LEVEL()

DESCRIPTION
  This function will register both the level as well as its opposite level
  passed to this function by the user for the optimized scenario. An example
  would be to register DSM_MEM_LEVEL_SIO_MANY and DSM_MEM_LEVEL_SIO_FEW 
  as a pair. An optimized scenario for a pair means a callback is not called 
  each time the threshold is crossed. Instead the callback is only called 
  when the corresponding "opposite" level has already been hit.

DEPENDENCIES
  Only 1 unique level/opposite_level pair can co-exist. For example, if 
  DSM_MEM_LEVEL_SIO_MANY has already been registered for an optimized
  callback with DSM_MEM_LEVEL_SIO_FEW, it cannot exists as a pair 
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
extern void dsm_reg_mem_event_optimized_level
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type  level,
  dsm_mem_level_enum_type  opposite_level
);

/*===========================================================================
FUNCTION DSM_REG_MEM_EVENT_OPTIMIZED_LEVEL_COMBINED()

DESCRIPTION
  This is a combined API which calls dsm_reg_mem_event_level for both levels
  belonging to a pair and then calls dsm_reg_mem_event_optimized_level to 
  enable optimized callback for them

DEPENDENCIES

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
extern void dsm_reg_mem_event_optimized_level_combined
(
  dsm_mempool_id_type pool_id,
  dsm_mem_level_enum_type level,
  uint32 avail_item_count_level,
  dsm_mem_level_enum_type opposite_level,
  uint32 avail_item_count_opposite_level
);
#endif /* DSM_POOL_H */
