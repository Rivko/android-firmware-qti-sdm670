#ifndef _TST_TOPOLOGY_DB_H_
#define _TST_TOPOLOGY_DB_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct audproc_module_info_t audproc_module_info_t;

struct audproc_module_info_t
{
   uint32_t module_id;
   uint32_t use_lpm;
   uint8_t  init_params[16];
};

typedef struct audproc_topology_definition_t audproc_topology_definition_t;

struct audproc_topology_definition_t
{
   uint32_t topology_id;
   uint32_t num_modules;
   audproc_module_info_t module_info[16];
};

typedef struct audproc_custom_topologies_t audproc_custom_topologies_t;

struct audproc_custom_topologies_t
{
   uint32_t num_topologies;
};

/** Success. The operation completed with no errors. */
#define ADSP_EOK          ( 0x00000000 )
/** General failure. */
#define ADSP_EFAILED      ( 0x00000001 )
/** Bad operation parameter. */
#define ADSP_EBADPARAM    ( 0x00000002 )
/** Unsupported routine or operation. */
#define ADSP_EUNSUPPORTED ( 0x00000003 )
/** Unsupported version. */
#define ADSP_EVERSION     ( 0x00000004 )
/** Unexpected problem encountered. */
#define ADSP_EUNEXPECTED  ( 0x00000005 )
/** Unhandled problem occurred. */
#define ADSP_EPANIC       ( 0x00000006 )
/** Unable to allocate resource. */
#define ADSP_ENORESOURCE  ( 0x00000007 )
/** Invalid handle. */
#define ADSP_EHANDLE      ( 0x00000008 )
/** Operation is already processed. */
#define ADSP_EALREADY     ( 0x00000009 )
/** Operation is not ready to be processed. */
#define ADSP_ENOTREADY    ( 0x0000000A )
/** Operation is pending completion. */
#define ADSP_EPENDING     ( 0x0000000B )
/** Operation could not be accepted or processed. */
#define ADSP_EBUSY        ( 0x0000000C )
/** Operation aborted due to an error. */
#define ADSP_EABORTED     ( 0x0000000D )
/** Operation preempted by a higher priority. */
#define ADSP_EPREEMPTED   ( 0x0000000E )
/** Operation requests intervention to complete. */
#define ADSP_ECONTINUE    ( 0x0000000F )
/** Operation requests immediate intervention to complete. */
#define ADSP_EIMMEDIATE   ( 0x00000010 )
/** Operation is not implemented. */
#define ADSP_ENOTIMPL     ( 0x00000011 )
/** Operation needs more data or resources. */
#define ADSP_ENEEDMORE    ( 0x00000012 )

/** Operation does not have memory. */
#define ADSP_ENOMEMORY     ( 0x00000014 )

/** Item does not exist. */
#define ADSP_ENOTEXIST     ( 0x00000015 )

/** Operation is finished. */
#define ADSP_ETERMINATED   ( 0x00011174 )

/** Status messages (error codes) returned by all aDSP command responses. */
typedef int ADSPResult;

#define ADSP_FAILED(x) (ADSP_EOK != (x))
#define ADSP_SUCCEEDED(x) (ADSP_EOK == (x))


typedef enum
{
   QURT_ELITE_HEAP_DEFAULT=0,
}  QURT_ELITE_HEAP_ID;

void *qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId);
void qurt_elite_memory_free(void *ptr);
typedef int qurt_elite_mutex_t;
static inline void qurt_elite_mutex_init(qurt_elite_mutex_t *pqurt_elite_mutex)
{
}

static inline void qurt_elite_mutex_lock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
}

static inline void qurt_elite_mutex_unlock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
}

static inline void qurt_elite_mutex_destroy(qurt_elite_mutex_t *pqurt_elite_mutex)
{
}

enum bool_t
{
   FALSE = 0,
   TRUE = 1
};

#define DBG_MSG(ssid, priority, message, ...) \
  do { \
      printf(" " message "\n", ##__VA_ARGS__); \
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


#endif /* _TST_TOPOLOGY_DB_H_ */
