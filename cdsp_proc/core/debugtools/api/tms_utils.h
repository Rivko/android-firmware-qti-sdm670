#ifndef TMS_UTILS_H
#define TMS_UTILS_H
/**
@file tms_utils.h
@brief This file contains the API for the TMS Utilities API 0.x
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014,2017-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/api/tms_utils.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/**

All function implementations as part of this API must remain reentrant. The
routines specifically do not use any NHLOS API which will block, or otherwise
serialize access to the data parameters.

It is the caller's responsiblity to handle data resource contention issues
as necessary. No routine within this API will internally supply resource
management for supplied data parameters.

*/

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_TMS_UTILS)                                             /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_TMS_UTILS                                                 /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_TMS_UTILS                                                  /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_TMS_UTILS                                                 /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_TMS_UTILS                                                 /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "ctype.h"
#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

#include "DALSys.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** @addtogroup tms_utils_api @{ */

/**
API, Typedefs and Enumerations
*/

typedef char tms_chr;                                                            /**< NULL Terminated Buffer */
typedef bool tms_bool;                                                           /**< Boolean */

typedef enum
{
   TMS_UTILS_STATUS_SUCCESS = 0,                                                 /**< SUCCESS INDICATION, evaluates as >= TMS_UTILS_STATUS_SUCCESS */
   TMS_UTILS_STATUS_ERROR = -1                                                   /**< FAILURE INDICATION, evaluates as < TMS_UTILS_STATUS_SUCCESS */

} TMS_UTILS_STATUS;

/**

API Information for tms_utils_chr_*()

Input Result Buffers will be both NULL Terminated and Size Bounded
Output Result Buffers will always be NULL Terminated and Size Bounded

Example Initializers

tms_chr const* return_result;
tms_chr const buf[] = "const string initializer";
tms_chr out_buf[OUT_BUF_SIZ];

*/

/**
API, NULL Terminated Buffer Append, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_append(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Compare

if (0 == tms_utils_chr_compare(buf, sizeof(buf), ...)) match;
if (0 > tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;
if (0 < tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int32_t tms_utils_chr_compare(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Copy, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_copy(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, First Occur of Param

if (NULL != tms_utils_chr_first(buf, sizeof(buf), 'r')) found;
if (NULL == tms_utils_chr_first(buf, sizeof(buf), 'r')) not_found;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr                       Input Character
@return        tms_chr*                      Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_chr const* tms_utils_chr_first(tms_chr const* in_buf_p, size_t in_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Search, Last Occur of Param

if (NULL != tms_utils_chr_last(buf, sizeof(buf), 'r')) found;
if (NULL == tms_utils_chr_last(buf, sizeof(buf), 'r')) not_found;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr                       Input Character
@return        tms_chr*                      Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_chr const* tms_utils_chr_last(tms_chr const* in_buf_p, size_t in_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Length

if (sizeof(buf) > tms_utils_chr_length(buf, sizeof(buf))) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_length(buf, sizeof(buf))) buffer_overflow;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_length(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, First Occurrance of Second Buffer Match

if (NULL != tms_utils_chr_run(buf, sizeof(buf), buf2, sizeof(buf2))) segment_pointer;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        tms_chr*                      Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_chr const* tms_utils_chr_run(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Search, First Occurrance of Any Byte From Second Buffer

if (NULL != tms_utils_chr_sep(buf, sizeof(buf), buf2, sizeof(buf2))) segment_pointer;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        tms_chr*                      Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_chr const* tms_utils_chr_sep(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Initializer

if (sizeof(buf) > tms_utils_chr_set(buf, sizeof(buf), chr)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_set(buf, sizeof(buf), chr)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr                       Output Buffer Initializer
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_set(tms_chr* out_buf_p, size_t out_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Search, Length of Initial Segment Comprised of Bytes from Second Buffer

if (0 != tms_utils_chr_span(buf, sizeof(buf), buf2, sizeof(buf2))) segment_length;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_span(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Search, Length of Initial Segment Not Comprised of Bytes from Second Buffer

if (0 != tms_utils_chr_span_n(buf, sizeof(buf), buf2, sizeof(buf2))) segment_length_result;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_span_n(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, Formatted Output to Sized Buffer

NOTE: Caller of routine supplies NHLOS protections to Output Buffer if
required. The routine itself does not carry any specific NHLOS details.
This routine supports only the following template expansions, nothing
more.

Supported Template Format Expansions

   %[-][0][ww][type]

   [-] left justification within field
   [0] zero pad numeric output
   [www] field width
   [type] field type
      c  character argument
      s  NULL terminated buffer pointer argument
      b  binary uint32_t argument
      d  decimal int32_t argument
      o  octal uint32_t argument
      x  hex uint32_t argument
      ld decimal int32_t argument
      lx hex uint32_t argument
      X  hex uint32_t argument (caps expansion)
      lX hex uint32_t argument (caps expansion)

Use Case Example

   tms_chr buf[256];
   int32_t size;

   if (sizeof(buf) > (size = tms_utils_fmt(buffer, sizeof(buffer), "%d", 12345)))
   {
      PRINT("%d, %d == strlen(\"%s\")\n", count, strlen(buffer), buffer);
   }
   else
   {
      ERROR(buf_overflow);
   }
*/

/** @addtogroup tms_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char TMS_UTILS_CHR_T;
typedef char* TMS_UTILS_BUF_P;
typedef const int TMS_UTILS_BUF_SZ;
typedef const char* TMS_UTILS_FMT_P;
typedef union { unsigned long ul; long l; unsigned u; int i; } TMS_UTILS_ARG_T;
typedef int TMS_UTILS_RET_T;

#define TMS_UTILS_BUFFER_SIZE       80
#define TMS_UTILS_BUF_SZ_ZERO       0
#define TMS_UTILS_RET_SUCCESS       0
#define TMS_UTILS_RET_ERROR         -1
#define TMS_UTILS_NULL              (void*)0

/**
API, Formatted Output to a Sized Buffer
NOTE: Caller of routine supplies NHLOS protections to Output Buffer if
required. The routine itself does not carry any specific NHLOS details.
@param[in]  TMS_UTILS_BUF_P   Output Buffer Pointer (NULL Terminated on Output)
@param[in]  TMS_UTILS_BUF_SZ  Output Buffer Size
@param[in]  TMS_UTILS_FMT_P   Format Template Pointer (NULL Terminated)
@param[in]  ...               Format Template Expansion Arguments
@return     TMS_UTILS_RET_T   Byte Count Output into Buffer Pointer (>=0) w/ NULL Terminator, Error Indications (<0)
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((format(printf, 3, 4)))
#endif
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS TMS_UTILS_RET_T tms_utils_fmt(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_str, ...);

/**
API, Formatted Output to a Sized Buffer
NOTE: This routine treats string fmt (%s) as an integer (%x)
@param[in]  TMS_UTILS_BUF_P   Output Buffer Pointer (NULL Terminated on Output)
@param[in]  TMS_UTILS_BUF_SZ  Output Buffer Size
@param[in]  TMS_UTILS_FMT_P   Format Template Pointer (NULL Terminated)
@param[in]  ...               Format Template Expansion Arguments
@return     TMS_UTILS_RET_T   Byte Count Output into Buffer Pointer (>=0) w/ NULL Terminator, Error Indications (<0)
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((format(printf, 3, 4)))
#endif
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS TMS_UTILS_RET_T tms_utils_fmt_nostring(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_tpl, ...);

/**
API, Generic FIFO Byte Buffer, Setup for Opaque List Handle

static buf[1024]; // FIFO Byte Store

tms_utils_chr_fifo_t fifo;

tms_chr chr;

if (0 <= tms_utils_chr_fifo_init(&fifo, buf, sizeof(buf))) chr_fifo_success;

if (0 <= tms_utils_chr_fifo_put(&fifo, 'r')) chr_fifo_success;

if (0 <= tms_utils_chr_fifo_get(&fifo, &chr)) chr_fifo_success;

*/

typedef struct                                                                   /**< INTERNAL, Used as Opaque Handle */
{
  size_t size;                                                                   /**< INTERNAL, Managed FIFO Size */
  tms_chr* buf_p;                                                                /**< INTERNAL, Managed FIFO Store */

  size_t offset_get;                                                             /**< INTERNAL, Managed Get Byte */
  size_t offset_put;                                                             /**< INTERNAL, Managed Put Byte */

} tms_utils_chr_fifo_t, *tms_utils_chr_fifo_p;

/**
API, FIFO Byte Buffer, Initialize

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@param[in]     tms_chr*                      FIFO Byte Buffer Backing Store Pointer
@param[in]     size_t                        FIFO Byte Buffer Backing Store Size
@return        int32_t                       Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int32_t tms_utils_chr_fifo_init(tms_utils_chr_fifo_p fifo_p, tms_chr* buf_p, size_t size);

/**
API, FIFO Byte Buffer, Store FIFO Byte

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        int32_t                       Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
int32_t tms_utils_chr_fifo_put(tms_utils_chr_fifo_p fifo_p, tms_chr chr);

/**
API, FIFO Byte Buffer, Retrieve FIFO Byte

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@param[in]     tms_chr*                      FIFO Byte Result Pointer
@return        int32_t                       Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int32_t tms_utils_chr_fifo_get(tms_utils_chr_fifo_p fifo_p, tms_chr* chr_p);

/**
API, FIFO Byte Buffer, FIFO Buffer Actual Size

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
size_t tms_utils_chr_fifo_size(tms_utils_chr_fifo_p fifo_p);

/**
API, FIFO Byte Buffer, FIFO Buffer Current Fill Count

@param[in]     tms_utils_chr_fifo_p          FIFO Byte Buffer Management Pointer
@return        size_t                        Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS size_t tms_utils_chr_fifo_elements(tms_utils_chr_fifo_p fifo_p);

/**
API, Generic Linked List, Setup for Opaque List Handle

TMS_UTILS_LIST(name);

{
   struct { int i; } *data;

   tms_utils_list_init(name);
   data = malloc(sizeof(*data)); data->i = 0;

   FIFO
   tms_utils_list_head_push(name, data);
   if (NULL == (data = tms_utils_head_pop(name))) fifo_empty;

   LIFO
   tms_utils_list_head_push(name, data);
   if (NULL == (data = tms_utils_tail_pop(name))) lifo_empty;
}

*/

typedef void** tms_utils_list_t; /**< Opaque List Handle */

#define TMS_UTILS_LIST(name) static void* name##_list = NULL; static tms_utils_list_t name = (tms_utils_list_t)&name##_list
#define TMS_UTILS_LIST_STRUCT(name) void* name##_list; tms_utils_list_t name
#define TMS_UTILS_LIST_STRUCT_INIT(struct_p, name) do { (struct_p)->name = &((struct_p)->name##_list); (struct_p)->name##_list = NULL; tms_utils_list_init((struct_p)->name); } while(0)

/**
API, Generic Linked List, Duplicate List

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     tms_utils_list_t              Opaque List Handle
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_duplicate(tms_utils_list_t list_dst, tms_utils_list_t list_src);

/**
API, Generic Linked List, Initialization

@param[in]     tms_utils_list_t              Opaque List Handle
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_init(tms_utils_list_t list);

/**
API, Generic Linked List, Return Head

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS void* tms_utils_list_head_peek(tms_utils_list_t list);

/**
API, Generic Linked List, Head Pop, Drop

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS void* tms_utils_list_head_pop(tms_utils_list_t list);

/**
API, Generic Linked List, Head Push, Add

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_head_push(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Items In List

@param[in]     tms_utils_list_t              Opaque List Handle
@return        int                           Item Count of List
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int tms_utils_list_length(tms_utils_list_t list);

/**
API, Generic Linked List, Delete From List

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_middle_delete(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Insert Within List

@param[in]     void*                         Data Payload Pointer
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_middle_insert(tms_utils_list_t list, void* item_prev, void* item_new);

/**
API, Generic Linked List, Tail Item

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS void* tms_utils_list_tail_peek(tms_utils_list_t list);

/**
API, Generic Linked List, Tail Pop, Drop

@param[in]     tms_utils_list_t              Opaque List Handle
@return        void*                         Data Payload Pointer
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS void* tms_utils_list_tail_pop(tms_utils_list_t list);

/**
API, Generic Linked List, Tail Push, Append

@param[in]     tms_utils_list_t              Opaque List Handle
@param[in]     void*                         Data Payload Pointer
@return        NONE                          No Return
*/

DLL_API_EXPORT_TMS_UTILS void tms_utils_list_tail_push(tms_utils_list_t list, void* item);

/**
API, Generic Linked List, Item Following Current

@param[in]     void*                         Data Payload Pointer
@return        void*                         Data Payload Pointer
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS void* tms_utils_list_item_next(void* item);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

tms_chr const buf[] = "665";
if (665 == tms_utils_num_int32(buf, sizeof(buf))) parsed_decimal;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Numeric Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int32_t tms_utils_num_int32(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

tms_chr const buf[] = "12345678";
if (0x12345678 == tms_utils_num_uint32(buf, sizeof(buf))) parsed_hex;

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS uint32_t tms_utils_num_uint32(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

tms_chr const buf[] = "50000";
if (50000 == tms_utils_num_int64(buf, sizeof(buf))) parsed_decimal;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int64_t                       Numeric Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS int64_t tms_utils_num_int64(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

tms_chr const buf[] = "aabbccdd";
if (0xaabbccdd == tms_utils_num_uint64(buf, sizeof(buf))) parsed_hex;

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint64_t                      Numeric Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS uint64_t tms_utils_num_uint64(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, Utility Hash Function

tms_chr const buf[] = "string initializer";
uint32_t hash = tms_utils_hash(buf, sizeof(buf));

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS uint32_t tms_utils_hash(tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, Generic Counting Semaphore

tms_utils_sem_t sem;

if (NULL != tms_utils_sem_init(&sem)) sem_init_success;

if (NULL != tms_utils_sem_wait(&sem)) sem_wait_success;

if (NULL != tms_utils_sem_signal(&sem)) sem_signal_success;

*/

typedef struct tms_utils_evtq_s
{
   DALSYSEventHandle hEvent;

   struct tms_utils_evtq_s* next;

} tms_utils_evtq_t, *tms_utils_evtq_p;

typedef struct tms_utils_sem_s
{
   DALSYSSyncHandle hSync;

   int count;

   tms_utils_evtq_p evtq;

} tms_utils_sem_t, *tms_utils_sem_p;

/**
API, Utility Counting Semaphore, Initialize

@param[in]     tms_utils_sem_p               Semaphore Structure
@return        tms_utils_sem_p               Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_utils_sem_p tms_utils_sem_init(tms_utils_sem_p const sem_p);

/**
API, Utility P/V Semaphore, P(), Acquires Semaphore (Blocks for > 0)

@param[in]     tms_utils_sem_p               Semaphore Structure
@return        tms_utils_sem_p               Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_utils_sem_p tms_utils_sem_wait(tms_utils_sem_p const sem_p);

/**
API, Utility P/V Semaphore, V(), Releases Semaphore (Increments by 1)

@param[in]     tms_utils_sem_p               Semaphore Structure
@return        tms_utils_sem_p               Operation Result
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_TMS_UTILS tms_utils_sem_p tms_utils_sem_signal(tms_utils_sem_p const sem_p);

/**
API, Get System Timer value for the TMS Timetick handle
@param[in]     uint64*                       Pointer to timetick type used to get timer value
@return        void                          Best effort function, no failure action
*/
DLL_API_EXPORT_TMS_UTILS void tms_get_timetick(uint64* ticktype_p);

/**
API, CPU and DDR vote.
*/
DLL_API_EXPORT_TMS_UTILS void tms_utils_vote_cpu_ddr(void);

/**
API, CPU and DDR unvote.
*/
DLL_API_EXPORT_TMS_UTILS void tms_utils_unvote_cpu_ddr(void);

/** @} end_addtogroup tms_utils_api */

#if defined(__cplusplus)
}
#endif

#endif
