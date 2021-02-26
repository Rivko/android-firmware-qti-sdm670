#ifndef QURT_ALLOC_H
#define QURT_ALLOC_H

/**
  @file qurt_alloc.h 
  @brief Prototypes of Kernel memory allocation API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2011-02-25   op      Add Header file
   2012-12-16   cm      (Tech Pubs) Edited/added Doxygen comments and markup.

 ======================================================================*/

/*======================================================================*/

/**@ingroup func_qurt_malloc
  Dynamically allocates the specified array on the QuRT system heap.
  The return value is the address of the allocated memory area.

  @note1hang The allocated memory area is automatically initialized to zero.

  @param[in] size     Size (in bytes) of the memory area.
  
  @return
  Nonzero -- Pointer to the allocated memory area. \n
  0 -- Not enough memory in heap to allocate memory area.

  @dependencies
  None.    

 */
/* ======================================================================*/
void *qurt_malloc( unsigned int size);

/*======================================================================*/
/**@ingroup func_qurt_calloc
  Dynamically allocates the specified array on the QuRT system heap.
  The return value is the address of the allocated array. 

  @note1hang The allocated memory area is automatically initialized to zero.

  @param[in] elsize Size (in bytes) of each array element.
  @param[in] num    Number of array elements.

  @return 
  Nonzero -- Pointer to allocated array.\n
  Zero -- Not enough memory in heap to allocate array.

  @dependencies
  None.
  
 */
 /* ======================================================================*/
void *qurt_calloc(unsigned int elsize, unsigned int num);

/*======================================================================*/
/**@ingroup func_qurt_realloc
  Reallocates memory on the heap. \n
  Changes the size of a memory area that is already allocated on the QuRT system heap.

  @note1hang This function may change the address of the memory area.
             If the value of ptr is NULL, this function is equivalent to 
             qurt_malloc().
             If the value of new_size is 0, it is equivalent to qurt_free().  
             If the memory area is expanded, the added memory is not initialized.

  @param[in] *ptr   Pointer to the address of the memory area.
  @param[in] newsize Size (in bytes) of the re-allocated memory area.
	               	
  @return
  Nonzero -- Pointer to re-allocated memory area. \n
  0 -- Not enough memory in heap to re-allocate the memory area.

  @dependencies
  None.
	 
 */
 /* ======================================================================*/
void *qurt_realloc(void *ptr,  int newsize);

/*======================================================================*/
/**@ingroup func_qurt_free
  Frees allocated memory from the heap.\n
  Deallocates the specified memory from the QuRT system heap.

  @param[in] *ptr Pointer to the address of the memory to be deallocated.
	
  @return
  None.

  @dependencies
  The memory item specified by the ptr value must have been previously 
             allocated using one of the memory allocation functions (qurt_calloc, 
             qurt_malloc, qurt_realloc). Otherwise the behavior of QuRT is undefined.
  
 */
 /* ======================================================================*/
void qurt_free( void *ptr);


void *qurt_memalign(unsigned int alignment, unsigned int size);

#endif /* QURT_ALLOC_H */

