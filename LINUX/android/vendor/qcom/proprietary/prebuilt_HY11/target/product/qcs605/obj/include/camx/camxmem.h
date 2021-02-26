////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxmem.h
/// @brief Declarations of macros and functions for memory allocation and freeing.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE NC010: Operator new and delete start with lower case letter
// NOWHINE FILE CF028: Many cases due to preprocessor macros that have to have parenthesis on own line
// NOWHINE FILE CP040: This file defines CAMX_NEW/CAMX_DELETE
// NOWHINE FILE DC011: Whiner is confused by new and delete overloading

#ifndef CAMXMEM_H
#define CAMXMEM_H

#include "camxdefs.h"
#include "camxtypes.h"

// Calling convention for Windows should be __cdecl and no calling convention on other OSes
#ifdef _WINDOWS
#define NEWDECL __cdecl
#else // _WINDOWS
#define NEWDECL
#endif // _WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief CamxMemFlags bit flags.  These are flags to pass into memory allocators.
typedef INT32             CamxMemFlags;
static const CamxMemFlags CamxMemFlagsNone            =  0x00;   ///< No flags
static const CamxMemFlags CamxMemFlagsAllocZeroMemory =  0x01;   ///< Memset the allocated space to 0
static const CamxMemFlags CamxMemFlagsDoNotTrack      =  0x02;   ///< Do not track this allocation
static const CamxMemFlags CamxMemFlagsArray           =  0x04;   ///< Buffer is an array
static const CamxMemFlags CamxMemFlagsNew             =  0x08;   ///< New was called (as opposed to malloc/calloc)
static const CamxMemFlags CamxMemFlagsDelete          =  0x10;   ///< Delete was called (as opposed to free)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory allocation for C and C++ code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC
///
/// @brief  Allocates and zero-initializes specified number of bytes.
///
/// @param  numBytes Number of bytes to allocate
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_CALLOC(numBytes) \
    CamxCalloc((numBytes), sizeof(VOID*), CamxMemFlagsAllocZeroMemory, CamxMemTypeAny, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_CALLOC(numBytes) \
    CamxCalloc((numBytes), sizeof(VOID*), CamxMemFlagsAllocZeroMemory, CamxMemTypeAny, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC_TYPED
///
/// @brief  Allocates and zero-initializes specified number of bytes and associates a type.
///
/// @param  numBytes    Number of bytes to allocate
/// @param  type        The type of memory to allocate. See CamxMemType.
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_CALLOC_TYPED(numBytes, type) \
    CamxCalloc((numBytes), sizeof(VOID*), CamxMemFlagsAllocZeroMemory, (type), __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_CALLOC_TYPED(numBytes, type) \
    CamxCalloc((numBytes), sizeof(VOID*), CamxMemFlagsAllocZeroMemory, (type), NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC_NO_SPY
///
/// @brief  Allocates and zero-initializes specified number of bytes. The allocation will not be tracked.
///
/// @param  numBytes Number of bytes to allocate
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_CALLOC_NO_SPY(numBytes) \
    CamxCalloc((numBytes), sizeof(VOID*), CamxMemFlagsAllocZeroMemory | CamxMemFlagsDoNotTrack, CamxMemTypeAny, NULL, 0)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC_ALIGNED
///
/// @brief  Allocates and zero-initializes specified number of bytes with specified alignment.
///
/// @param  numBytes    Number of bytes to allocate
/// @param  alignment   The byte alignment required. This must be a power of 2.
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_CALLOC_ALIGNED(numBytes, alignment) \
    CamxCalloc((numBytes), (alignment), CamxMemFlagsAllocZeroMemory, CamxMemTypeAny, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_CALLOC_ALIGNED(numBytes, alignment) \
    CamxCalloc((numBytes), (alignment), CamxMemFlagsAllocZeroMemory, CamxMemTypeAny, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC_ALIGNED_TYPED
///
/// @brief Allocates and zero-initializes specified number of bytes with specified alignment, and associates a type.
///
/// @param numBytes     Number of bytes to allocate
/// @param alignment   The byte alignment required. This must be a power of 2.
/// @param type        The type of memory to allocate. See CamxMemType.
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_CALLOC_ALIGNED_TYPED(numBytes, alignment, type) \
    CamxCalloc((numBytes), (alignment), CamxMemFlagsAllocZeroMemory, (type), __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_CALLOC_ALIGNED_TYPED(numBytes, alignment, type) \
    CamxCalloc((numBytes), (alignment), CamxMemFlagsAllocZeroMemory, (type), NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_CALLOC_ALIGNED_NO_SPY
///
/// @brief  Allocates and zero-initializes specified number of bytes with specified alignment. The allocation will not be
///         tracked.
///
/// @param  numBytes    Number of bytes to allocate
/// @param  alignment   The byte alignment required. This must be a power of 2.
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_CALLOC_ALIGNED_NO_SPY(numBytes, alignment)                                                                 \
    CamxCalloc((numBytes), (alignment), CamxMemFlagsAllocZeroMemory | CamxMemFlagsDoNotTrack, CamxMemTypeAny, NULL, 0)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_FREE
///
/// @brief  Frees the memory pointed to by the pointer passed in.
///
/// @param  ptr Memory pointer to free
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_FREE(ptr) \
    CamxFree((ptr), CamxMemFlagsNone, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_FREE(ptr) \
    CamxFree((ptr), CamxMemFlagsNone, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_FREE_NO_SPY
///
/// @brief  Frees the memory pointed to by the pointer passed in. The free will not be tracked.
///
/// @param  ptr Memory pointer to free
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_FREE_NO_SPY(ptr) \
    CamxFree((ptr), CamxMemFlagsDoNotTrack, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_FREE_NO_SPY(ptr) \
    CamxFree((ptr), CamxMemFlagsDoNotTrack, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_FREE_ALIGNED
///
/// @brief  Frees the memory pointed to by the aligned pointer passed in.
///
/// @param  ptr Memory pointer to free
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_FREE_ALIGNED(ptr) \
    CamxFree((ptr), CamxMemFlagsNone, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_FREE_ALIGNED(ptr) \
    CamxFree((ptr), CamxMemFlagsNone, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_FREE_ALIGNED_NO_SPY
///
/// @brief  Frees the memory pointed to by the pointer passed in. The free will not be tracked.
///
/// @param  ptr Memory pointer to free
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_FREE_ALIGNED_NO_SPY(ptr) \
    CamxFree((ptr), CamxMemFlagsDoNotTrack, NULL, 0)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxCalloc
///
/// @brief  Method to allocate and optionally track memory. Do not use this directly, use CAMX_CALLOC* instead.
///
/// @param  numBytes    Number of bytes to allocate
/// @param  alignment   The byte alignment required. This must be a power of 2. A value of 0 indicates no alignment required.
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  type        The type of the memory allocation.
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return non-NULL pointer if successful
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* CamxCalloc(
    SIZE_T          numBytes,
    UINT32          alignment,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxFree
///
/// @brief  Method to free and optionally track memory. Do not use this directly, use CAMX_FREE* instead.
///
/// @param  pMem        Memory pointer to free
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CamxFree(
    VOID*           pMem,
    CamxMemFlags    flags,
    const CHAR*     pFilename,
    UINT            lineNum);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory allocation for C++ code only
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// new/delete Macros
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_NEW
///
/// @brief  The CamX placement new macro used to instantiate C++ class objects.
///
/// Allocates memory for an instance of the class and calls the class constructor for object initialization.
///
/// @return A pointer to the newly instantiated class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_NEW new(CamxMemFlagsNone, CamxMemTypeAny, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_NEW new(CamxMemFlagsNone, CamxMemTypeAny, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_NEW_TYPED
///
/// @brief  The CamX placement new macro used to instantiate C++ class objects using the memory type specified.
///
/// Allocates memory with the specified type for an instance of the class and calls the class constructor for object
/// initialization.
///
/// @param  type The type of the memory allocation
///
/// @return A pointer to the newly instantiated class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_NEW_TYPED(type) new(CamxMemFlagsNone, type, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_NEW_TYPED(type) new(CamxMemFlagsNone, type, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_NEW_NO_SPY
///
/// @brief  The CamX placement new macro used to instantiate C++ class objects. The allocation will not be tracked.
///
/// Allocates untracked memory for an instance of the class and calls the class constructor for object initialization.
///
/// @return A pointer to the newly instantiated class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CAMX_USE_MEMSPY
#define CAMX_NEW_NO_SPY new(CamxMemFlagsDoNotTrack, CamxMemTypeAny, __FILE__, __LINE__)
#else // CAMX_USE_MEMSPY
#define CAMX_NEW_NO_SPY new(CamxMemFlagsDoNotTrack, CamxMemTypeAny, NULL, 0)
#endif // CAMX_USE_MEMSPY

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_DELETE
///
/// @brief  The CamX delete macro used to delete the object instantiated by the placement new.
///
/// Calls the class destructor and frees the memory used by the instance of the class. There is no placement delete in C++, we
/// must either call delete directly or use our own method to destroy the object and deallocate memory. For now, call delete.
///
/// @param  pObject The pointer to the object to delete.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_DELETE delete

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CAMX_DELETE_NO_SPY
///
/// @brief  The CamX delete macro used to delete the object instantiated by the placement new. The delete will not be tracked.
///
/// Calls the class destructor and frees the memory used by the instance of the class. There is no placement delete in C++, we
/// must either call delete directly or use our own method to destroy the object and deallocate memory. For now, call delete.
///
/// @param  pObject The pointer to the object to delete.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAMX_DELETE_NO_SPY delete

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// new/delete Overrides
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// new
///
/// @brief  The CamX placement operator new override used to instantiate C++ class objects. Do not use this directly, use the
///         CAMX_NEW macro instead.
///
/// @param  numBytes    The number of bytes to allocate on the system heap
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  type        The type of the memory allocation
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return A pointer to the object instance on the heap, if successful. NULL otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID* NEWDECL operator new(
    size_t          numBytes,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID* NEWDECL operator new(
    size_t          numBytes,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum)
{
    return CamxCalloc(
        numBytes,
        sizeof(VOID*),
        flags | CamxMemFlagsAllocZeroMemory | CamxMemFlagsNew,
        type,
        pFilename,
        lineNum
        );
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// new
///
/// @brief  The operator new override provided for completeness. This should never be used. Instead, use the CAMX_NEW macro that
///         wraps the placement operator new.
///
/// @param  numBytes The number of bytes to allocate on the system heap
///
/// @return A pointer to the object instance on the heap, if successful. NULL otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID* NEWDECL operator new(
    size_t numBytes);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID* NEWDECL operator new(
    size_t numBytes)
{
    return CamxCalloc(
        numBytes,
        sizeof(VOID*),
        CamxMemFlagsAllocZeroMemory | CamxMemFlagsNew,
        CamxMemTypeAny,
        NULL,
        0);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// new[]
///
/// @brief  The CamX placement operator new[] override used to instantiate an array of C++ class objects. Do not use this
///         directly, use the CAMX_NEW macro instead.
///
/// @param  numBytes    The number of bytes to allocate on the system heap
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  type        The type of the memory allocation
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return A pointer to the object instance on the heap, if successful. NULL otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID* NEWDECL operator new[](
    size_t          numBytes,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID* NEWDECL operator new[](
    size_t          numBytes,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum)
{
    return CamxCalloc(
        numBytes,
        sizeof(VOID*),
        flags | CamxMemFlagsAllocZeroMemory | CamxMemFlagsArray | CamxMemFlagsNew ,
        type,
        pFilename,
        lineNum);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// new[]
///
/// @brief  The operator new[] override provided for completeness. This should never be used. Instead, use the CAMX_NEW macro
///         that wraps the placement operator new.
///
/// @param  numBytes The number of bytes to allocate on the system heap
///
/// @return A pointer to the object instance on the heap, if successful. NULL otherwise.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID* NEWDECL operator new[](
    size_t numBytes);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID* NEWDECL operator new[](
    size_t numBytes)
{
    return CamxCalloc(
        numBytes,
        sizeof(VOID*),
        CamxMemFlagsAllocZeroMemory | CamxMemFlagsArray | CamxMemFlagsNew,
        CamxMemTypeAny,
        NULL,
        0);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// delete
///
/// @brief  The placement operator delete override provided for completeness. This will only be used by placement new when an
///         object's constructor throws an exception. CAMX_DELETE* macros will never invoke this operator delete override.
///
/// @param  pObject     The pointer to the object to delete
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  type        The type of the memory allocation
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID NEWDECL operator delete(
    VOID*           pObject,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID NEWDECL operator delete(
    VOID*           pObject,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum)
{
    // The 'type' parameter is added here because the VS compiler requires the exact same signature for placement new and
    // placement delete. For now, we can't use it.
    CAMX_UNREFERENCED_PARAM(type);

    // The only incoming flag we would be interested in for this placement delete would be if we do not want to track it
    CamxFree(
        pObject,
        (flags & CamxMemFlagsDoNotTrack) | CamxMemFlagsDelete,
        pFilename,
        lineNum);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// delete
///
/// @brief  The CamX operator delete override used to delete C++ class objects. Do not use this directly, use the
///         CAMX_DELETE macro instead which will invoke this operator delete override.
///
/// @param  pObject The pointer to the object to delete
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID NEWDECL operator delete(
    VOID* pObject);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID NEWDECL operator delete(
    VOID* pObject)
{
    CamxFree(
        pObject,
        CamxMemFlagsDelete,
        NULL,
        0);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// delete[]
///
/// @brief  The placement operator delete[] override provided for completeness. This will only be used by placement new[] when
///         an object's constructor throws an exception. CAMX_DELETE* macros will never invoke this operator delete[] override.
///
/// @param  pObject     The pointer to the object to delete
/// @param  flags       Flags to indicate the heap operations taking place
/// @param  type        The type of the memory allocation
/// @param  pFilename   The filename of the callsite for this function
/// @param  lineNum     The line number of the callsite for this function
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID NEWDECL operator delete[](
    VOID*           pObject,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum
    );
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID NEWDECL operator delete[](
    VOID*           pObject,
    CamxMemFlags    flags,
    CamxMemType     type,
    const CHAR*     pFilename,
    UINT            lineNum)
{
    // The 'type' parameter is added here because the VS compiler requires the exact same signature for placement new[] and
    // placement delete[]. For now, we can't use it.
    CAMX_UNREFERENCED_PARAM(type);

    // The only incoming flag we would be interested in for this placement delete would be if we do not want to track it
    CamxFree(
        pObject,
        (flags & CamxMemFlagsDoNotTrack) | CamxMemFlagsArray | CamxMemFlagsDelete,
        pFilename,
        lineNum);
}
#endif // __GNUC__ || _MSC_VER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// delete[]
///
/// @brief  The CamX operator delete[] override used to delete an array of C++ class objects. Do not use this directly, use the
///         CAMX_DELETE macro instead which will invoke this operator delete[] override.
///
/// @param  pObject The pointer to the object to delete
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __clang__ && !defined(LE_CAMERA)
VOID NEWDECL operator delete[](
    VOID* pObject);
#elif (__GNUC__ || _MSC_VER)
CAMX_INLINE VOID NEWDECL operator delete[](
    VOID* pObject)
{
    CamxFree(
        pObject,
        CamxMemFlagsArray | CamxMemFlagsDelete,
        NULL,
        0);
}
#endif // __GNUC__ || _MSC_VER

#endif // __cplusplus

#endif // CAMXMEM_H
