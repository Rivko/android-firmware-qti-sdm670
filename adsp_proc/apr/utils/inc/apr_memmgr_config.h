#ifndef __APR_MEMMGR_CONFIG_H__
#define __APR_MEMMGR_CONFIG_H__

#define FEATURE_APR_MEMMGR_MT /* Enable multi-threading support. */
#if 0
#define FEATURE_APR_MEMMGR_DEBUG    /* Enable memory debugging support. Covers consistency and asserts. Pulls in crc.h, comdef.h, amssassert.h. */
#define FEATURE_APR_MEMMGR_DEBUG_EX /* Enable extended memory debugging support. Covers crash dumps. Uses TCB_SELF(). Requires FEATURE_MEM_DEBUG. */
#endif /* 0 */
#define FEATURE_APR_MEMMGR_HEAP_SMALLER_OVERHEAD ( 1 ) /* 1: Enable smaller heap block overheads with possibly more fragmentation. */

#endif /* __APR_MEMMGR_CONFIG_H__ */

