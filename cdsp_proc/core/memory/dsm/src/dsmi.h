#ifndef DSMI_H
#define DSMI_H
/*===========================================================================

                                  D S M I . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1.c4/memory/dsm/src/dsmi.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/2011  rj     Internalize private fields within watermark structure   
06/09/2010  hh     Included INTLOCK/INTFREE for L4 feature.
02/22/10    ag     DSM CMI changes.
01/01/05    pjb    Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm_lock.h"
#include "dsm_queue.h"


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
#define DSM_MIN_MAX_CNT(wm_ptr,trace_array) dsm_min_max_cnt(wm_ptr,trace_array)


#define DSM_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
     }



/* ------------- Runtime Debug Features ------------- */

#ifdef FEATURE_DSM_MEM_CHK
/* Only fill freed items with AA if this is non-zero. */
extern boolean DSM_Fill_AA;
#endif /* FEATURE_DSM_MEM_CHK */

/* --------- Cookies --------- */
/* Cookie must have this value */
#define DSM_COOKIE                             (0x4EAD4EAD)
#define DSM_TAIL_COOKIE                        (0xF007F007)
#define DSM_POOL_MGMT_TABLE_COOKIE             (0x7AB1E5E7)

#ifdef FEATURE_DSM_MEM_CHK
  #define DSM_TAIL_COOKIE_LEN                  (4) 
  #define DSM_HEAD_COOKIE_LEN                  (4)
#else
  #define DSM_TAIL_COOKIE_LEN                  (0) 
  #define DSM_HEAD_COOKIE_LEN                  (0)
#endif /* FEATURE_DSM_MEM_CHK */


/* a single dsm item can not hold more than 65535 bytes of data */
#define DSM_POOL_MAX_ITEM_SIZE ((uint16)-1)


#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE

/* Most EXPENSIVE memory checks require the basic DSM_MEM_CHK feature
 * be enabled
 */
#ifndef FEATURE_DSM_MEM_CHK
#define FEATURE_DSM_MEM_CHK
#endif

/* Expensive check for double free packet */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE
#endif

/*  Verify all packets for all operations */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
#endif

/*  Verify packets on enqueue */
#ifndef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
#define FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
#endif

/* Verify that IOV packets are not duplicates. This check
 * will fail on some targets
 */
//#define FEATURE_DSM_MEM_CHK_EXPENSIVE_IOV_DUP


/* Verify that wm and actual bytes on queue match up */
//#define FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH

/* Verify that wm and actual bytes on queue match up */
//#define FEATURE_DSM_MEM_CHK_QUEUE_CACHE_LENGTH


#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE */

/*---------------------------------------------------------------------------
  Watermark Trace Structure definition. This internal structure provides the 
  means to store various control flow variables for a watermark. It is only 
  enabled if FEATURE_DSM_WM_TRACING is defined
---------------------------------------------------------------------------*/

typedef struct dsm_watermark_trace_s
{
 
  uint32  min_cnt;               /* Minimum item count maintained for
                                    each window period */

  uint32  max_cnt;               /* Maximum item count maintained for
                                    each window period */

  uint8  hi_event_cnt;           /* Number of times high watermark is 
                                    hit during a period. */

  uint8  lo_event_cnt;           /* Number of times low watermark is 
                                    hit during a period. */

  uint8  dne_event_cnt;          /* Number of times dne count is 
                                    reached during a period. */

  uint8  empty_event_cnt;        /* Number of times the watermark goes
                                    empty during a period. */

} dsm_watermark_trace_type;


/*===========================================================================
                      MACRO DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
MACRO DSMI_POOL_OBJECT_SIZE(item_size,pad_size)

DESCRIPTION
  This takes the size of an item and calculates the size of the whole 
  item with overhead.

  In DSM 3 this should be a multiple of 32.  Item size should already be a 
  multiple of 32. 
---------------------------------------------------------------------------*/
#define DSMI_POOL_OBJECT_SIZE(item_size, pad_size)\
 ( sizeof(dsm_item_type) +  \
   DSM_HEAD_COOKIE_LEN   +  \
   item_size             +  \
   DSM_TAIL_COOKIE_LEN   +  \
   pad_size )




/*---------------------------------------------------------------------------
MACRO DSMI_ITEM_HEAD(item_ptr)

DESCRIPTION
  Return the address of the first bit of data for this item
---------------------------------------------------------------------------*/
#define DSMI_ITEM_HEAD(item_ptr)                                            \
  ((uint8*)(((uint8 *)((item_ptr) + 1)) + DSM_HEAD_COOKIE_LEN))



/*---------------------------------------------------------------------------
MACRO DSMI_ITEM_TAIL(item_ptr)

DESCRIPTION
 Return the address just past the last bit of data for this item. 
---------------------------------------------------------------------------*/
#define DSMI_ITEM_TAIL(item_ptr)                                            \
  (DSMI_ITEM_HEAD(item_ptr) +                                               \
   DSM_POOL_ITEM_SIZE(DSM_ITEM_POOL(item_ptr)))



/*---------------------------------------------------------------------------
MACRO DSMI_DUP(item_ptr)

DESCRIPTION
 Return true if this item is from a being used as a DUP or from is from
 a DUPLICATE pool.
---------------------------------------------------------------------------*/
#define DSMI_DUP(item_ptr)                            \
  ( (item_ptr)->dup_ptr != NULL ||                    \
    DSM_POOL_ITEM_SIZE(DSM_ITEM_POOL(item_ptr)) == 0 )

#define DSMI_IMUTABLE(item_ptr)			      \
  ( ((item_ptr)->references != 1) || DSMI_DUP(item_ptr) )



/*---------------------------------------------------------------------------
MACRO DSMI_HEAD_SIZE(item_ptr)

DESCRIPTION
  Returns the size between the head pointer and the data pointer
---------------------------------------------------------------------------*/
#define DSMI_HEAD_SIZE(item_ptr) \
  ( (item_ptr)->data_ptr - DSMI_ITEM_HEAD(item_ptr) )



/*---------------------------------------------------------------------------
MACRO DSMI_TAIL_SIZE(item_ptr)

DESCRIPTION
  Returns the size of unused space between the current data and the end
  of the packet.
---------------------------------------------------------------------------*/
#define DSMI_TAIL_SIZE(item_ptr) \
  ( (DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr) - (item_ptr)->used )



/*---------------------------------------------------------------------------
MACRO DSMI_SIZE(item_ptr)

DESCRIPTION
  Returns the value in the local size field of the item, or what it should
  be if the size field is not defined for the dsm_item_type.
---------------------------------------------------------------------------*/
#define DSMI_SIZE(item_ptr) \
  ( DSMI_DUP(item_ptr) ? (item_ptr)->used : \
    ( DSMI_ITEM_TAIL(item_ptr) - (item_ptr)->data_ptr ) )



/*---------------------------------------------------------------------------
MACRO DSM_NOTUSED(x)

DESCRIPTION
  Tricks lint into thinking that the variable is used when it's not
---------------------------------------------------------------------------*/
#define DSM_NOTUSED(i) if(i){}


/* -------------------------------------------------------------------------
MACRO DSMI_POOL_LOCK(pool), DSMI_POOL_UNLOCK(pool) 
 
DESCRIPTION 
  Macros for obtaining and releasing the pool mutex.
---------------------------------------------------------------------------*/ 
#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_POOL_LOCK( pool ) \
    DSM_LOCK( &(((dsm_pool_mgmt_table_type*)(pool))->lock) ) 
#elif defined FEATURE_L4
  #define DSMI_POOL_LOCK( pool ) \
    INTLOCK() 
#else 
  #define DSMI_POOL_LOCK( pool ) \
    INTLOCK(); \
    TASKLOCK()
#endif    

#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_POOL_UNLOCK( pool ) \
    DSM_UNLOCK( &(((dsm_pool_mgmt_table_type*)(pool))->lock) )
#elif defined FEATURE_L4 
  #define DSMI_POOL_UNLOCK( pool ) \
    INTFREE()
#else 
  #define DSMI_POOL_UNLOCK( pool ) \
    TASKFREE(); \
    INTFREE() 
#endif 
  
/* -------------------------------------------------------------------------
MACRO DSMI_QUEUE_LOCK_WM(pool), DSMI_QUEUE_UNLOCK_WM(pool) 
 
DESCRIPTION 
  Macros for obtaining and releasing the watermark mutex.
---------------------------------------------------------------------------*/ 

#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    DSM_LOCK( &((wm_ptr)->lock) )
#elif defined FEATURE_L4 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    INTLOCK()
#else 
  #define DSMI_QUEUE_LOCK_WM(wm_ptr) \
    INTLOCK(); \
    TASKLOCK()
#endif  
  
#if defined FEATURE_DSM_WINCE || \
    defined FEATURE_DSM_QUBE  || \
    defined FEATURE_DSM_NATIVE_LINUX || \
    defined FEATURE_DSM_POSIX || \
    defined FEATURE_DSM_REX
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    DSM_UNLOCK( &((wm_ptr)->lock) )
#elif defined FEATURE_L4  
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    INTFREE() 
#else
  #define DSMI_QUEUE_UNLOCK_WM(wm_ptr) \
    TASKFREE(); \
    INTFREE()
#endif   

#define DSMI_QUEUE_LOCK_CREATE(wm_ptr) \
  DSM_LOCK_CREATE( &((wm_ptr)->lock) ); \
  (wm_ptr)->lock_init = TRUE

#define DSMI_QUEUE_LOCK_DESTROY(wm_ptr) \
  DSM_LOCK_DESTROY( &((wm_ptr)->lock) )

/* -------------------------------------------------------------------------
MACRO DSMI_INCR_COUNT(wm_ptr,count_type), DSMI_MIN_MAX_CNT(wm_ptr),
      DSMI_GET_COUNT_ARR( wm_ptr )

DESCRIPTION 
  Macros used for updating Watermark Trace structure.
---------------------------------------------------------------------------*/ 
#define DSMI_INCR_COUNT(wm_ptr,count_type) \
  (((dsm_watermark_trace_type*)((wm_ptr)->reserved))->count_type##_event_cnt++)

#define DSMI_MIN_MAX_CNT(wm_ptr) \
  DSM_MIN_MAX_CNT(wm_ptr,(dsm_watermark_trace_type*)((wm_ptr)->reserved)) 

#define DSMI_GET_TRACE_ARR(wm_ptr) \
  ((dsm_watermark_trace_type*)((wm_ptr)->reserved)) 

/*---------------------------------------------------------------------------
                            FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
uint8 dsmi_ref_inc(dsm_item_type * item_ptr);
uint8 dsmi_ref_dec(dsm_item_type * item_ptr);
void dsmi_verify_buffer(dsm_item_type * item_ptr);
void dsmi_verify_pool_id(dsm_mempool_id_type pool_id);
void dsmi_verify_mem_level(dsm_mem_level_enum_type mem_level);
void dsmi_verify_mem_op(dsm_mem_op_enum_type mem_op);
void dsmi_check_mem_all_pools(void);
/*---------------------------------------------------------------------------
                            FUNCTION DECLARATIONS
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION DSM_MIN_MAX_CNT()

DESCRIPTION
   This function updates the min_cnt and max_cnt of the watermark queue
   that is passed.
 
DEPENDENCIES

PARAMETERS
   wm_ptr - Pointer to the watermark queue.
   trace_array - Starting address of the array where the trace counts 
                 are stored.
RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void dsm_min_max_cnt
(
  dsm_watermark_type*  wm_ptr, 
  dsm_watermark_trace_type*  trace_array
);
#endif /* DSMI_H */
