#ifndef _CAPI_V2_UTILS_H
#define _CAPI_V2_UTILS_H
/*===========================================================================

                  C A P I V 2  U T I L I T I E S

  Common definitions, types and functions for CAPIv2.

   Copyright (c) 2013, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/capi_v2_cmn_lib/inc/capi_v2_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2013/09/03 dg      Created file
===========================================================================*/

#ifdef CAPI_V2_STANDALONE
/* -----------------------------------------------------------------------
** Standard include files
** ----------------------------------------------------------------------- */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#if defined(__cplusplus)
#include <new>
extern "C" {
#endif // __cplusplus
#include<assert.h>

/* -----------------------------------------------------------------------
** ADSP and QURT Elite include files
** ----------------------------------------------------------------------- */
#include "adsp_audproc_api.h"
#include "adsp_asm_api.h"
#include "hexagon_sim_timer.h"

/* -----------------------------------------------------------------------
** Standard Integer Types and Definitions
** ----------------------------------------------------------------------- */
typedef unsigned long long uint64_t;  /* Unsigned 64 bit value */
typedef unsigned long int  uint32_t;  /* Unsigned 32 bit value */
typedef unsigned short     uint16_t;  /* Unsigned 16 bit value */
typedef unsigned char      uint8_t;   /* Unsigned 8  bit value */

typedef signed long long   int64_t;   /* Signed 64 bit value */
typedef signed long int    int32_t;   /* Signed 32 bit value */
typedef signed short       int16_t;   /* Signed 16 bit value */
typedef signed char        int8_t;    /* Signed 8  bit value */

#undef TRUE
#undef FALSE

#define TRUE   (1)  /* Boolean true value. */
#define FALSE  (0)  /* Boolean false value. */

#ifndef NULL
#define NULL (0)
#endif

/* ------------------------------------------------------------------------
 ** QURT ELITE Memory Definitions
 ** ------------------------------------------------------------------------ */
typedef enum
{
   QURT_ELITE_HEAP_DEFAULT=0,         
   QURT_ELITE_HEAP_OUT_OF_RANGE      //keep at end
}  QURT_ELITE_HEAP_ID;

void* qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId);
void qurt_elite_memory_free(void *ptr);

#if defined(__cplusplus)
#define qurt_elite_memory_placement_new(pObj, pMemory, typeType, ...) { \
   (pObj) = (new (pMemory) (typeType)(__VA_ARGS__)); \
}

#define qurt_elite_memory_placement_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); (pObject) = NULL; }\
}

#define qurt_elite_memory_new(pObject, typeType, heapId, ...) { \
   void *pObj = qurt_elite_memory_malloc(sizeof(typeType), heapId); \
   (pObject) = (pObj) ? (new (pObj) (typeType)(__VA_ARGS__)) : NULL; \
}

#define qurt_elite_memory_delete(pObject, typeType) { \
   if (pObject) { (pObject)->~typeType(); \
   qurt_elite_memory_free (pObject); \
   (pObject) = NULL; } \
}

#endif // __cplusplus
/* ------------------------------------------------------------------------
 ** QURT ELITE Diag Definitions
 ** ------------------------------------------------------------------------ */
#define debug_enable    1                  /**< disable or enable debug msg*/
#define lowest_priority DBG_MED_PRIO       /**< lowest debug message level to be shown*/

#define DBG_FATAL_PRIO 16
#define DBG_ERROR_PRIO 8
#define DBG_HIGH_PRIO  4
#define DBG_MED_PRIO   2
#define DBG_LOW_PRIO   1

#define MSG_SSID_QDSP6  0

//if __FILENAME macro is not available, just leave that field blank
#ifndef __FILENAME__
#define __FILENAME__ ""
#endif

// assert is for priority == 16 (i.e on FATAL)
// disable warning "conditional expression is constant" for debug_enable
#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (debug_enable && (priority >= lowest_priority)) \
    { \
      uint64_t cycles = hexagon_sim_read_cycles(); \
      int tms = ((cycles >> 10)*1805)>>20; \
      int tsec = ((cycles >> 10)*1805)>>30; \
      int tmin = ((cycles >> 10)*30)>>30; \
      printf(" %dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
      if (16 == priority) QURT_ELITE_ASSERT(0);\
    } \
  } while(0) \

#define MSG(xx_ss_id, xx_ss_mask, xx_fmt)                       DBG_MSG(0,xx_ss_mask,xx_fmt)
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1)            DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1)
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)   DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1,xx_arg2)
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)


#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
    DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)


#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_QDSP6, DBG_FATAL_PRIO, x_fmt, a, b, c)

/* Original macro cut&paste from AudCmnUtil.h, leave it outside SURF for kernel debugging */
/* This macro prints to QURT buffer */
/* Print macro for debugging */
#define PRINT()   printf("%s %d\n",__FILENAME__, __LINE__);

/*
   Qurt_elite version of assert.
*/
#define QURT_ELITE_ASSERT(x) assert(x)

/* ------------------------------------------------------------------------
 ** Audio Post Processing Definitions
 ** ------------------------------------------------------------------------ */

#define s32_min_s32_s32(var1, var2)  Q6_R_min_RR(var1,var2)

#define   LL_shr(x, n)       (Q6_P_asr_PR((x),(n)))

#define   extract_h(x)       ((int16)Q6_R_asrh_R( (x) ))       

#define   L_sat(x)           (Q6_R_sat_P( (x) ))

#define PCM_16BIT_Q_FORMAT              15  /* Q15 */
#define ELITE_32BIT_PCM_Q_FORMAT        27  /* Q27 */

static size_t  memscpy(
          void        *dst,
          size_t      dst_size,
          const void  *src,
          size_t      src_size
          )
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;

  memcpy(dst, src, copy_size);

  return copy_size;
}

static size_t memsmove(
          void        *dst,
          size_t      dst_size,
          const void  *src,
          size_t      src_size
          )
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;

  memmove(dst, src, copy_size);

  return copy_size;
}

#if defined(__cplusplus)
} // extern "C"
#endif

#else  // CAPI_V2_STANDALONE
#include "qurt_elite.h"
#include "adsp_audproc_api.h"
#include "adsp_asm_api.h"

#endif // CAPI_V2_STANDALONE
#endif /* _CAPI_V2_UTILS_H */

