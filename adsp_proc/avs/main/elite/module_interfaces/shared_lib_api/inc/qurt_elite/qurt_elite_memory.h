/*========================================================================
Copyright (c) 2010-2011, 2013-2014, 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */
/**
@file qurt_elite_memory.h

@brief This file contains utilities for memory allocation and release. This
file provides memory allocation functions and macros for both C and C++.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_memory_intro
The following utilities include memory allocation functions and macros for
both C and C++
 - qurt_elite_memory_malloc()
 - qurt_elite_memory_free()
 - qurt_elite_memory_aligned_malloc()
 - qurt_elite_memory_aligned_free()
 - qurt_elite_memory_new()
 - qurt_elite_memory_delete()
 - qurt_elite_memory_placement_new()
 - qurt_elite_memory_placement_delete()
 - #QURT_ELITE_FREE()
 - #QURT_ELITE_ALIGNED_FREE()
 - qurt_elite_memory_heapmgr_malloc()
 - qurt_elite_memory_heapmgr_free()
 - qurt_elite_memory_heapmgr_create()
 - qurt_elite_memory_heapmgr_delete()
 - qurt_elite_heap_table_init()
 - qurt_elite_memory_stats_init()
 - qurt_elite_memory_stats_deinit()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_memory.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw      (Tech Pubs) General edits in Doxygen comments.
09/08/14    sw     (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/11   ss       qurt_elite Placement new/delete
03/08/10   brs      Edited for doxygen-to-latex process.
02/04/10   mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_MEMORY_H
#define QURT_ELITE_MEMORY_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_types.h"

#ifdef __cplusplus
#include <new>
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_memory
@{ */
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/** Maximum number of heaps supported in the Elite framework. */
#define QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS  10

/** Index of the default heap. @newpage */
#define QURT_ELITE_DEFAULT_HEAP_INDEX 0

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

//***************************************************************************
// Heap features
//***************************************************************************

/** ID of the available heap in the system.
*/
typedef enum
{
   QURT_ELITE_HEAP_DEFAULT=0,
   /**< Default heap value. */

   QURT_ELITE_HEAP_OUT_OF_RANGE=QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS+1
   /**< Heap value is out of range. @newpagetable */ //keep at end

}  QURT_ELITE_HEAP_ID;

/**
  Allocates memory with an option to specify a heap identifier.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] unBytes Number of bytes to allocate.
  @param[in] heapId   ID of the heap from which to allocate memory.

  @return
  Pointer to the allocated block, or NULL if the request failed.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void *qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId);

/**
  Frees memory that was allocated with qurt_elite_memory_malloc().

  @param[in] ptr  Pointer to the memory to free.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_memory_free(void *ptr);

/**
  Allocates memory with options to align to a power of 2 and to
  specify a heap identifier.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] unBytes      Number of bytes to allocate.
  @param[in] unAlignBits  Number of alignment bytes.
                          This value must be a power of 2.
  @param[in] heapId       ID of the heap from which to allocate memory.

  @return
  Pointer to the allocated block, or NULL if the request failed.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void *qurt_elite_memory_aligned_malloc(uint32_t unBytes, uint32_t unAlignBits, QURT_ELITE_HEAP_ID heapId);

/**
  Frees memory that was allocated with qurt_elite_memory_aligned_malloc().

  @param[in] ptr Pointer to the aligned memory to free.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_memory_aligned_free(void *ptr);

#ifdef __cplusplus

/**
  Allocates a C++ object using qurt_elite_memory_malloc().

  This macro takes the pointer, type, and heap ID and does the following:
  - Allocates memory
  - Follows with a <i>placement new</i>, which calls the constructor of the
    object

  This macro takes extra arguments that can be passed to the constructor
  of the C++ object. This does not cause any exceptions. If the
  allocation fails, the returned pointer is NULL.

  @param[out] pObject   Pointer to set to the new object location.
  @param[in]  typeType  Type of class to construct.
  @param[in]  heapId    QURT_ELITE_HEAP_ID is requested for allocating the
                        new object.
  @param[in]  ...       Variable argument list for parameters that are
                        passed to the constructor.
  @hideinitializer
*/
#define qurt_elite_memory_new(pObject, typeType, heapId, ...) { \
   void *pObj = qurt_elite_memory_malloc(sizeof(typeType), heapId); \
   (pObject) = (pObj) ? (new (pObj) (typeType)(__VA_ARGS__)) : NULL; \
}

/**
  Destroys an object created with qurt_elite_memory_new().
  After destroying the object, the macro frees the memory and sets the
  pointer to NULL.

  @param[in,out] pObject   Pointer to the object to be destroyed.
  @param[in]     typeType  Class of the object to be destroyed.

  @hideinitializer @newpage
*/
#define qurt_elite_memory_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); \
   qurt_elite_memory_free (pObject); \
   (pObject) = NULL; } \
}

/**
  Macro that takes a pointer to the intended class, a pointer to the allocated
  memory, and the type of class, and then invokes the constructor of the class.

  This macro can take extra arguments that might be passed to the
  constructor of the C++ object. This does not cause any exceptions.

  @param[out] pObj      Pointer to the intended class whose constructor
                        is to be invoked.
  @param[in]  pMemory   Pointer to memory that has been allocated.
  @param[in]  typeType  Type of class to construct.
  @param[in]  ...       Variable argument list for parameters passed to
                        the constructor.

  @hideinitializer
*/
#define qurt_elite_memory_placement_new(pObj, pMemory, typeType, ...) { \
   (pObj) = (new (pMemory) (typeType)(__VA_ARGS__)); \
}

/**
  Destroys an object created with qurt_elite_memory_placement_new().
  This macro invokes the destructor of the intended class and sets the
  pointer to NULL.

  @param[in,out] pObject   Pointer to the object to be destroyed.
  @param[in]     typeType  Class of the object to be destroyed.

  @hideinitializer
*/
#define qurt_elite_memory_placement_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); (pObject) = NULL; }\
}

/** Utility macro that frees a pointer and then sets the pointer to NULL.
    @hideinitializer
*/
#define QURT_ELITE_FREE(ptr) \
do { \
qurt_elite_memory_free(ptr); \
ptr = NULL; \
} while(0)

/** Utility macro that frees a pointer and then sets the
    pointer to NULL. This macro is to be used for memory that was allocated
    with aligned memory allocation.
    @hideinitializer
*/
#define QURT_ELITE_ALIGNED_FREE(ptr) \
do { \
qurt_elite_memory_aligned_free(ptr); \
ptr = NULL; \
} while(0)

#endif //__cplusplus for new and delete


/**
  Allocates memory in the heap.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] size     Number of bytes to allocate.
  @param[in] heap_id  ID of the heap.

  @return
  Pointer to the allocated buffer.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void* qurt_elite_memory_heapmgr_malloc(uint32_t size, QURT_ELITE_HEAP_ID heap_id);

/**
  Frees allocated memory.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] ptr      Pointer to the buffer to be freed.
  @param[in] heap_id  ID of the heap.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_memory_heapmgr_free(void *ptr, QURT_ELITE_HEAP_ID heap_id);

/**
  Initializes the heap manager for a specified heap.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] heap_id_ptr     Pointer to the heap ID.
  @param[in] heap_start_ptr  Pointer to the start address of the heap.
  @param[in] heap_size       Size of the heap.

  @return
  Status of the heap manager creation.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
ADSPResult qurt_elite_memory_heapmgr_create(QURT_ELITE_HEAP_ID *heap_id_ptr, void *heap_start_ptr, uint32_t heap_size);

/**
  De-initializes the heap manager of a specified heap ID.

  @datatypes
  #QURT_ELITE_HEAP_ID

  @param[in] heap_id  ID of the heap.

  @return
  Status of the heap manager deletion.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ADSPResult qurt_elite_memory_heapmgr_delete(QURT_ELITE_HEAP_ID heap_id);

/**
  Initializes the heap table entries for the heap manager.

  @return
  None.

  @dependencies
  None.
*/
void qurt_elite_heap_table_init(void);

/**
  Initializes the variables used for maintaining memory usage statistics for
  AVS and non-AVS threads.

  @return
  None.

  @dependencies
  None.
*/

void qurt_elite_memory_stats_init(void);

/**
  De-initializes the variables used for maintaining memory usage statistics for
  AVS and non-AVS threads.

  @return
  None.

  @dependencies
  None.
*/
void qurt_elite_memory_stats_deinit(void);

/** @} */ /* end_addtogroup qurt_elite_memory */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MEMORY_H

