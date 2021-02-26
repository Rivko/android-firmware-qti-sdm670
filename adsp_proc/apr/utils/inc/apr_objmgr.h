#ifndef __APR_OBJMGR_H__
#define __APR_OBJMGR_H__

/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/inc/apr_objmgr.h#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_list.h"   /* For the link-list implementation. */
#include "apr_comdef.h" /* For the standard POD types. */

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * OBJECT DEFINITIONS                                                       *
 ****************************************************************************/

/**< A union representing any plain-old-data type. */
typedef union apr_objmgr_any_pod_type_t
{
  int8_t i8;
  uint8_t u8;
  int16_t i16;
  uint16_t u16;
  int32_t i32;
  uint32_t u32;
  void* ptr;
}
  apr_objmgr_any_pod_type_t;

/**
 * The handle-object structure.
 */
typedef struct apr_objmgr_object_t
{
  apr_list_node_t link;
  uint32_t handle;
    /**<
     * \par The self-handle contains the object's current dynamic version
     * and index values.
     *
     * The general structure of the handle is:
     *
     *    Bn..Bi Bj Bk..B0
     *
     * where Bn..Bi is the version field, Bj is the visible flag, and Bk..B0
     * is the handle's index into the handle-object table. The value n is
     * the highest bit position in an atomic CPU word. Bn..Bi contains
     * between 0 to n-2 bits. Bj contains 1 bit. Bk..B0 contains between 1
     * to n-1 bits.
     *
     * Let's assume the configuration is 32 bits per word and 5 bits for
     * handle indicies. With this configuration the handle-object table
     * manages up to 32 active handles. There are 26 bits (= 32 bits
     * in a word - 1 bit for the visible flag - 5 bits for handle indicies
     * configuration) to spare. These 26 bits are used for handle-object
     * versioning.
     *
     * The full extension of 31 bits of version and handle indicies combined
     * allows for over 2 billion unique handle values before any handles
     * repeat. This expanded handle identifier space provides a window of
     * time for clients to recover or exit before unintended handle
     * recycling start causing problems.
     *
     * \par Implementation notes:
     *
     * The implementation should enforce an bijective handle to object
     * ownership relationship to control the lifetime of an object. The
     * owner has the right to directly access the object by pointer and has
     * the right to free the object through the object's handle.
     *
     * The implementation should allow surjective accesses to the objects.
     * Non-owners access objects through object handles while holding on to
     * any locks required to guaranty their lifetimes. The implementation
     * should minimize holding locks to reduce interrupt and thread
     * contentions.
     *
     * Object reference counters are not yet supported. This feature is
     * under research and may help with asynchronous resource sharing. After
     * the rules of concurrency safety and performance have been identified
     * we may revisit the rules of bijection and surjection.
     */
  uint32_t type;
    /**< User-defined value and behavior. */
  apr_objmgr_any_pod_type_t any;
    /**< User-defined value and behavior. */
}
  apr_objmgr_object_t;

/****************************************************************************
 * OBJECT SESSION DEFINITIONS                                               *
 ****************************************************************************/

typedef void ( *apr_objmgr_lock_fn_type )( void );
typedef void ( *apr_objmgr_unlock_fn_type )( void );

typedef struct apr_objmgr_t
{
  apr_list_t free_q;
    /**< The free object queue. */
  apr_objmgr_object_t* table;
    /**< The object storage table. */
  uint32_t total_num;
    /**<
     * The total number of handles in the table. Incidentally the total_num is
     * also the bit position of the visible flag.
     *
     * The total number of objects must be greater than 0 and less than or
     * equal to pow(2,n-1) where n is the number of bits in the CPU word.
     * The total number of objects must be a power of 2 value.
     *
     * Internally the total number of objects is used to calculate the
     * bitmask to retrieve the index into the object storage table from a
     * handle (right of the highest bit set in `total_num'); is used as the
     * value to set the visible flag; and, demarks the beginning of the
     * version field (left of the highest set bit of the `total_num').
     *
     * Normally handle identifier spaces are fixed between 0 and some limit
     * L, where L is the maximum number of handles possible in an
     * application. If L was small such as 9, then it is easy to reallocate
     * a handle value of 9 within 10 allocations and releases. This is not
     * good for error resiliency because a freed handle could be
     * unintentionally reused during the transient period of tear down. The
     * power of 2 partitioning increases the handle identifier space by
     * stretching handle values from a maximum of L to a constant n-1 word
     * bits. 31 bits are used for handles in a 32-bit word (one bit reserved
     * for the version flag). Even when the maximum active handles is 10, the
     * new system doesn't repeat handle values until about 2 billion times
     * later which makes reusing stale handle less probable.
     */
  uint32_t index_mask;
    /**< A bitmask to retrieve the index bits. */
  uint32_t overflow_mask;
    /**<
     * A bitmask used to perform manual wrap-around for handles less than 32
     * bits.
     */
  apr_objmgr_lock_fn_type lock_fn;
  apr_objmgr_unlock_fn_type unlock_fn;
}
  apr_objmgr_t;

typedef struct apr_objmgr_setup_params_t
{
  apr_objmgr_object_t* table;
    /**< The object storage table. */
  uint32_t total_bits;
    /**< The total number of bits used for generating handles. */
  uint32_t index_bits;
    /**< The number of bits partitioned for indicies. */
  apr_objmgr_lock_fn_type lock_fn;
    /**< The client supplied lock routine. */
  apr_objmgr_unlock_fn_type unlock_fn;
    /**< The client supplied unlock routine. */
}
  apr_objmgr_setup_params_t;

/****************************************************************************
 * EXPORTED ROUTINES                                                        *
 ****************************************************************************/

/**
 * Constructs an object manager.
 *
 * \param[in] objmgr The object manager structure.
 *
 * \return APR_EOK (0) when successful.
 */
APR_EXTERNAL int32_t apr_objmgr_construct (
  apr_objmgr_t* objmgr,
  apr_objmgr_setup_params_t* params
);

/**
 * Destructs an object manager.
 *
 * \param[in] objmgr The object manager structure.
 *
 * \return APR_EOK (0) when successful.
 */
APR_EXTERNAL int32_t apr_objmgr_destruct (
  apr_objmgr_t* objmgr
);

/**
 * Allocates a free object from the object manager.
 *
 * \param[in] objmgr The object manager structure.
 * \param[out] ret_object Returns a free object.
 *
 * \return APR_EOK (0) when successful.
 */
APR_EXTERNAL int32_t apr_objmgr_alloc_object (
  apr_objmgr_t* objmgr,
  apr_objmgr_object_t** ret_object
);

/**
 * Finds an object given a handle.
 *
 * \param[in] objmgr The object manager structure.
 * \param[in] handle The handle to find.
 * \param[out] ret_object Returns a reference to the found object.
 *
 * \return APR_EOK (0) when successful.
 */
APR_EXTERNAL int32_t apr_objmgr_find_object (
  apr_objmgr_t* objmgr,
  uint32_t handle,
  apr_objmgr_object_t** ret_object
);

/**
 * Frees an object given a handle.
 *
 * \param[in] objmgr The object manager structure.
 * \param[in] handle The handle to free.
 *
 * \return APR_EOK (0) when successful.
 */
APR_EXTERNAL int32_t apr_objmgr_free_object (
  apr_objmgr_t* objmgr,
  uint32_t handle
);

#endif /* __APR_OBJMGR_H__ */

