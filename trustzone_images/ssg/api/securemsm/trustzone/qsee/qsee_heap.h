#ifndef QSEEHEAP_H
#define QSEEHEAP_H

/**
@file qsee_heap.h
@brief Provide heap management API wrappers
*/


/*===========================================================================

DESCRIPTION
  Wrapper for secure malloc and free calls for secure heap.

Copyright (c) 2011  by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_heap.h#1 $
$DateTime: 2018/02/06 03:00:17 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include <stddef.h>
#include <stdint.h>
#include <string.h>


/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif

/**
 * @brief   Allocates a block of size bytes from the heap.  If size is 0, NULL will be returned.
 *
 * @param[in] size Number of bytes to allocate from the heap.
 *
 * @return   Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.
 *
 */
void* qsee_malloc(size_t size);

/**
 * @brief   Resizes a block of memory previously allocated using qsee_malloc() or qsee_realloc().  If size is zero, NULL will be returned.
 *
 * @param[in] pointer to previously allocated block. If NULL is passed this function is akin to qsee_malloc()
 *
 * @param[in] new size of block.
 *
 * @return   Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.
 *
 */
void* qsee_realloc(void* ptr, size_t size);

/**
 * @brief  Deallocates the ptr block of memory.
 *
 * @param[in] ptr pointer to block which will be freed
 *
 */
void qsee_free(void *ptr);

/**
 * @brief   Allocates and zero-intialize a block of size bytes.
 *
 * @param[in] size Number of bytes to allocate from the heap.
 *
 * @return   Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.
 *
 */
static inline void *qsee_zalloc(size_t size)
{
   void *p = qsee_malloc(size);
   if (p != NULL) {
      memset(p, 0, size);
   }
   return p;
}

/**
 * @brief Allocate and zero-initialize a block large enough to hold `num` items of size `size`.
 */
static inline void *qsee_calloc(size_t num, size_t size)
{
  if (num > SIZE_MAX/size) {
    return NULL;
  }
  return qsee_zalloc(num * size);
}



/**
 * @brief Allocate and zero-initialize a block large enough to hold an
 * object of the given type.
 */
#define QSEE_ZALLOC_TYPE(type)     ((type*) qsee_zalloc(sizeof(type)))

/**
 * @brief Allocate and zero-initialize a block large enough to hold an
 * array of n items of the given type.
 */
#define QSEE_ZALLOC_ARRAY(type, n) ((type *) qsee_calloc((n), sizeof(type)))

#define QSEE_FREE_PTR(var)         ((void) (qsee_free(var), (var) = 0))

#ifdef __cplusplus
   }
#endif

#endif /* QSEEHEAP_H */


