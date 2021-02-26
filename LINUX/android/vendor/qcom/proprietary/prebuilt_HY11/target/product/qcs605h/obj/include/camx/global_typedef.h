//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

/*========================================================================
GLOBAL_TYPEDEF . H

*//** @file global_typedef.h
Global type & macro defintions


*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/01/07   ab     Initial revision

========================================================================== */

#ifndef __GLOBAL_TYPEDEF_H__
#define __GLOBAL_TYPEDEF_H__
#ifdef __cplusplus
extern "C" {
#endif


    /*------------------------------------------------------------------------
     *       Include Files
     * ----------------------------------------------------------------------- */
#include <string.h>
#include <stdint.h>

    /*------------------------------------------------------------------------
    *       Definitions and Constants
    * ----------------------------------------------------------------------- */
#ifndef ABS
#define ABS(x)         ((x) >= 0 ? (x) : -(x))
#endif

#define CLIP1(x)       (!((x) & 0x100) ? (x) : (x) < 0 ? 0 : 255)
#define CLIP3(a, b, x) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))

#define CLAMP(X, Y) (X >= 0)? ((X <= Y)? X : Y) : 0

#define mmax(a, b)     ((a) > (b) ? (a) : (b))
#define mmin(a, b)     ((a) < (b) ? (a) : (b))
#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif  //FALSE

#ifndef TRUE
#define TRUE  1
#endif //TRUE

#ifndef FAIL
#define FAIL 0
#endif //FAIL

#ifndef SUCCESS
#define SUCCESS 1
#endif //SUCCESS

#define ALIGNMENT_8     8
#define ALIGNMENT_16    16
#define ALIGNMENT_32    32
#define ALIGNMENT_128   128

// A5 Cache Line Size (32-byte)
#define CACHE_LINE_SIZE (32)

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef LENGTH
#define  LENGTH( start, end ) ((end) - (start) + 1)
#endif

#ifndef CLZ
#define CLZ(reg, val) { reg = 0; while( reg < 32 && ((val & (1 << (31 - reg))) == 0) ) reg ++;  }
#endif

#define ISPOWER2(x) (0 == ((x) & ((x)-1)))

#ifndef SHRT_MAX
#define SHRT_MAX      32767         /* maximum (signed) short value */
#endif

#define CEIL_DIV(x,y)           (((x)+(y)-1)/(y))
#define ALIGN_UP(x,y)           (CEIL_DIV((x),(y))*(y))
#define ALIGN_DOWN(x,y)         ((x)-((x)%(y)))

#define ARRAY_LEN(arr)          (sizeof(arr)/sizeof(arr[0]))

#define ZERO_STRUCT_PTR(ptr)    memset((void*)(ptr), 0, sizeof(*(ptr)))
#define ZERO_ARRAY(ptr, count)  memset((void*)(ptr), 0, sizeof(*(ptr))*(count))

#define IS_MASK_ENABLE( __value, __mask) (((__value)&(__mask))?1:0)

    /*------------------------------------------------------------------------
     *       Type Declarations
     * ----------------------------------------------------------------------- */
    typedef int8_t      Int8;
    typedef uint8_t     UInt8;
    typedef int16_t     Int16;
    typedef uint16_t    UInt16;
    typedef int32_t     Int32;
    typedef uint32_t    UInt32;
    typedef int64_t     Int64;
    typedef uint64_t    UInt64;    /** Unsigned 64 bit type */
//    typedef int32_t     BOOL;
#define BOOL            int32_t

    typedef void * (AllocatorFuncPtr)(Int32 size);
    typedef void   (DeallocatorFuncPtr)(void *pMem);

    /// Structure for holding memory allocator, deallocator information
    typedef struct
    {
        AllocatorFuncPtr   *pfAlloc;        ///< Allocator func ptr
        void               *pAllocArg;      ///< Allocator func arg
        DeallocatorFuncPtr *pfDealloc;      ///< Deallocator func ptr
        void               *pDeallocArg;    ///< Deallocator func arg
    } vHeap;

    /*------------------------------------------------------------------------
     *       External Variables
     * ----------------------------------------------------------------------- */
    /*------------------------------------------------------------------------
     *       Function Declarations
     * ----------------------------------------------------------------------- */
    // Declaration of assert
#ifdef ELF_COMPILATION
    #undef assert  //disable ARM compiler defination
    #define assert(x) assert_loop((UInt32)(x), __LINE__, __FILE__)
    // incompatible redefinition of macro "assert" (declared
    // at line 66 of "C:\Apps\ARMCT5.01\43\include\assert.h")
    void sfr_init(void);
    void assert_loop(UInt32 condition, const UInt32 ln, const char *fn);
#endif

// Compilation-time assertion macro
#ifndef FW_CSIM_DLL
    #define COMPILE_TIME_ASSERT(expr) _COMPILE_TIME_ASSERT(expr, __LINE__)
    #define _COMPILE_TIME_ASSERT(expr, id)  \
        typedef char _PASTE_STR(CASSERT_FAILED_, id)[(expr)?1:-1]
    #define _PASTE_STR(a,b) a##b
#else
    /*disabled compile time asserts for FW dll because
      some checks are for pointer size == 32bits    */
    #define COMPILE_TIME_ASSERT(expr)
#endif/*FW_CSIM_DLL*/
// Safe function prototypes (Klocwork)
#if defined(_MSC_VER)
    #define memscpy(dst, dst_size, src, src_size)       \
        memcpy_s(dst, dst_size, src, src_size)
    #if _MSC_VER < 1900
    // in VS2015(1900), snprint if is already defined
    #define snprintf(dst, size, fmt, ...)               \
        sprintf_s((dst), (size), (fmt), ##__VA_ARGS__)
   #endif
#elif defined(__GNUC__)
    #define memscpy(dst, dst_size, src, src_size)       \
        memcpy(dst, src, src_size)
#else
    extern size_t memscpy(void *dst, size_t dst_size,   \
        const void *src, size_t src_size);
#endif

// Macro to indicate a deliberate infinite loop to suppress Klocwork errors
#define LOOP_FOREVER()          while(1)

// Macro for casting a value to an unrelated type
#define REINTERPRET_CAST(t,v)   ((t)(v))

// Macro to silence "declared but never referenced" warnings
#define USED_IN_DEBUG(arg)      ((void)(arg))

// Unqualified name of the encolsing function
#ifndef __func__
#define __func__                __FUNCTION__
#endif

#ifdef __GNUC__
    #define __forceinline       static inline __attribute__((always_inline))
    #define __NORETURN          __attribute__ ((noreturn))
#else
    #define __NORETURN          __declspec(noreturn)
#endif

#ifdef __cplusplus
}
#endif
#endif // __GLOBAL_TYPEDEF_H__


