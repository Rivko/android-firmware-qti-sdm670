
/******************************************************************************

                        N E T M G R _ U T I L . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_util.h
  @brief   Network Manager Utility Functions Header File

  DESCRIPTION
  Header file for NetMgr utility functions.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_UTIL_H__
#define __NETMGR_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <sys/select.h>
#ifndef FEATURE_DS_LINUX_ANDROID
#include <string.h>
#endif
#ifdef FEATURE_DATA_LOG_ADB
#include <android/log.h>
#include <utils/Log.h>
#include "common_log.h"
#endif
#include "comdef.h"
#include "ds_util.h"
#include "netmgr_defs.h"
#include "netmgr.h"
#include "wireless_data_service_v01.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
/* Macros for parameters*/
#define UNUSED(x)    (void)(x)


/* Macros for locking and unlocking a mutex */
#define NETMGR_LOCK_MUTEX(mutex)                                   \
  do                                                               \
  {                                                                \
    if (0 == pthread_mutex_lock(&mutex))                           \
    {                                                              \
      netmgr_log_med(">>>>>> LOCK MUTEX %p SUCCESS", &mutex);    \
    }                                                              \
    else                                                           \
    {                                                              \
      netmgr_log_err(">>>>>> LOCK MUTEX %p FAILURE", &mutex);      \
    }                                                              \
  }                                                                \
  while (0)

#define NETMGR_UNLOCK_MUTEX(mutex)                                 \
  do                                                               \
  {                                                                \
    if (0 == pthread_mutex_unlock(&mutex))                         \
    {                                                              \
      netmgr_log_med("<<<<<< UNLOCK MUTEX %p SUCCESS", &mutex);    \
    }                                                              \
    else                                                           \
    {                                                              \
      netmgr_log_err("<<<<<< UNLOCK MUTEX %p FAILURE", &mutex);    \
    }                                                              \
  }                                                                \
  while (0)

#define NETMGR_UTIL_MAX_CIRC_LIST_SIZE (10)

typedef struct
{
  void *data[NETMGR_UTIL_MAX_CIRC_LIST_SIZE+1];
  int r;
  int w;
} netmgr_util_circ_list_type;

#ifdef NETMGR_TEST
/* Suppress 'static' keyword in offtarget builds so testcases can
 * invoke internal functions. */
#define LOCAL
#define netmgr_malloc(size)  netmgr_debug_malloc(size)
#define netmgr_free(ptr)     netmgr_debug_free(ptr)
#define LOG(...)
#else
#ifdef LOCAL
#undef LOCAL
#define LOCAL static
#endif
#define netmgr_malloc(size)  malloc(size)
#define netmgr_free(ptr)     if( ptr ){ free(ptr); }
#endif /* NETMGR_TEST */

/*---------------------------------------------------------------------------
   Definition of 'assert' macro. This is needed as ONCRPC/DSM hijacks path
   to include another file with name assert.h, so that the standard library
   assert is not available to ONCRPC clients.
---------------------------------------------------------------------------*/
#define NETMGR_ASSERT(a)   ds_assert(a)
/*---------------------------------------------------------------------------
   Logging macros
---------------------------------------------------------------------------*/
#undef  DS_LOG_TAG
#define DS_LOG_TAG "QC-NETMGR-LIB"
#undef  LOG_TAG
#define LOG_TAG DS_LOG_TAG
/*---------------------------------------------------------------------------
  Definition of 'abort' macro.  Diagnostic messages are typically sent
  to QXDM but there is no guarantee message will be displayed.  Need
  to ensure abort related message is sent to persistent Android log.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LOG_ADB
#define NETMGR_ABORT(...)                            \
     netmgr_util_wakelock_force_release();           \
     LOGE( __VA_ARGS__);                             \
     fprintf(stderr, __VA_ARGS__);                   \
     exit(-1);
#else
#define NETMGR_ABORT(...)                            \
     netmgr_util_wakelock_force_release();           \
     fprintf(stderr, __VA_ARGS__);                   \
     exit(-1);
#endif
/*---------------------------------------------------------------------------
  Definition of 'stop' macro.  Diagnostic messages are typically sent
  to QXDM but there is no guarantee message will be displayed.  Need
  to ensure abort related message is sent to persistent Android log.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LOG_ADB
#define NETMGR_STOP(...)                             \
     LOGE( __VA_ARGS__);       \
     fprintf(stderr, __VA_ARGS__);                   \
     while(1) { sleep(0xFFFFFFFF); }
#else
#define NETMGR_STOP(...)                             \
     fprintf(stderr, __VA_ARGS__);                   \
     while(1) { sleep(0xFFFFFFFF); }
#endif

/*---------------------------------------------------------------------------
   Logging macros
---------------------------------------------------------------------------*/
#define NETMGR_LOG_FILE_LOCATION "/data/vendor/netmgr/log.txt"
extern FILE* logging_fd;
extern boolean log_to_file;

#define MAX_HMS_FMT_LEN   10
#define MAX_TIMESTAMP_LEN 25
#define TIMESTAMP(time_stamp)                               \
  struct    timeval tv;                                     \
  struct    tm *tmstmp;                                     \
  uint32_t  msec;                                           \
  char      tm_info[MAX_HMS_FMT_LEN];                       \
  gettimeofday(&tv, NULL);                                  \
  tmstmp = localtime(&tv.tv_sec);                           \
  msec = tv.tv_usec / 1000;                                 \
  strftime(tm_info, sizeof(tm_info), "%T", tmstmp);         \
  snprintf(time_stamp, sizeof(time_stamp), "%s.%" PRIu32 "",\
           tm_info,                                         \
           msec);

#define netmgr_log_fprintf(level,...)                                                      \
  do {                                                                                     \
    if(log_to_file && logging_fd)                                                          \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %30s():%3u (%s):",                                     \
              timestamp, __FILE__, __func__, __LINE__, ##level);                           \
      fprintf(logging_fd, __VA_ARGS__);                                                    \
      fprintf(logging_fd, "\n");                                                           \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
  } while (0);

#define  netmgr_log(...)                                                                   \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (INF):",                                    \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log(__VA_ARGS__);                                                                   \
  } while(0);

#define  netmgr_log_err(...)                                                               \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (ERR):",                                    \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_err(__VA_ARGS__);                                                               \
  } while(0);

#define  netmgr_log_high(...)                                                              \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (HIGH):",                                   \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_high(__VA_ARGS__);                                                              \
  } while(0);

#define  netmgr_log_med(...)                                                               \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (MED):",                                    \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_med(__VA_ARGS__);                                                               \
  } while(0);

#define  netmgr_log_low(...)                                                               \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (LOW):",                                    \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_low(__VA_ARGS__);                                                               \
  } while(0);

#define  netmgr_log_dflt(...)                                                              \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (DFLT):",                                   \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_dflt(__VA_ARGS__);                                                              \
  } while(0);

#define  netmgr_log_sys_err(...)                                                           \
  do {                                                                                     \
    if (log_to_file && logging_fd)                                                         \
    {                                                                                      \
      char timestamp[MAX_TIMESTAMP_LEN];                                                   \
      TIMESTAMP(timestamp);                                                                \
      fprintf(logging_fd,"%25s %70s %40s():%3u (ERR):",                                    \
              timestamp, __FILE__, __func__, __LINE__);                                    \
      fprintf(logging_fd,__VA_ARGS__);                                                     \
      fprintf(logging_fd,"\n");                                                            \
      fflush(logging_fd);                                                                  \
    }                                                                                      \
    ds_log_sys_err(__VA_ARGS__);                                                           \
  } while(0);

#define  netmgr_log_init          ds_log_init2

#define netmgr_l2s_max_msg_size 256
#define netmgr_l2s_invalid_link -1
#define netmgr_l2s_pname_size   50

extern char l2s_proc_name[netmgr_l2s_pname_size];
extern int  l2s_pid;

#define netmgr_l2s_setup()                                                   \
  do{                                                                        \
    l2s_pid = -1;                                                            \
    memset(l2s_proc_name,0, netmgr_l2s_pname_size);                          \
    l2s_pid = getpid();                                                      \
    ds_get_proc_name(l2s_pid,l2s_proc_name,netmgr_l2s_pname_size);           \
    netmgr_log_med("L2S[pname:%s pid:%d tid:%d]",                            \
                    l2s_proc_name,l2s_pid, gettid() );                       \
  }while (0);

#define netmgr_l2s_entry_params(fmt, ...)                                    \
  netmgr_log_med("L2S[pname:%s pid:%d tid:%d type:%s fname:%s params:" fmt "]",\
                  l2s_proc_name,                                             \
                  l2s_pid,                                                   \
                  gettid(),                                                  \
                  "FE",                                                      \
                  __func__,                                                  \
                  ##__VA_ARGS__);

#define netmgr_l2s_entry()   netmgr_l2s_entry_params("")

#define netmgr_l2s_exit_with_status(fmt,...)                                 \
  netmgr_log_med("L2S[pname:%s pid:%d tid:%d type:%s fname:%s status:%s params:" fmt "]",\
                  l2s_proc_name,                                             \
                  l2s_pid,                                                   \
                  gettid(),                                                  \
                  "FX",                                                      \
                  __func__,                                                  \
                  (ret==NETMGR_ERROR)?"ERROR":"SUCCESS",                     \
                  ##__VA_ARGS__);

#define netmgr_l2s_exit()                                                    \
  netmgr_log_med("L2S[pname:%s pid:%d tid:%d type:%s fname:%s]",             \
                  l2s_proc_name,                                             \
                  l2s_pid,                                                   \
                  gettid(),                                                  \
                  "FX",                                                      \
                  __func__);


#define netmgr_l2s_msg(iface_id, msg)                                        \
  netmgr_log_med("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",\
                  l2s_proc_name,                                             \
                  l2s_pid,                                                   \
                  gettid(),                                                  \
                  "ME",                                                      \
                  __func__,                                                  \
                  iface_id,                                                  \
                  msg);

#define netmgr_l2s_msg_seq(iface_id,msg)                                     \
  netmgr_log_med("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",\
                  l2s_proc_name,                                             \
                  l2s_pid,                                                   \
                  gettid(),                                                  \
                  "MS",                                                      \
                  __func__,                                                  \
                  iface_id,                                                  \
                  msg);

#define netmgr_l2s_format_msg(iface_id,msg,...)                              \
  do{                                                                        \
    char l2s_msg[netmgr_l2s_max_msg_size] = "\0" ;                           \
    memset(l2s_msg,0,netmgr_l2s_max_msg_size);                               \
    snprintf(l2s_msg,netmgr_l2s_max_msg_size,msg, __VA_ARGS__);              \
    netmgr_l2s_msg(iface_id,l2s_msg);                                        \
  }while (0);


extern boolean function_debug;
#define  NETMGR_LOG_FUNC_ENTRY    if(function_debug){ ds_log_func_entry(); }
#define  NETMGR_LOG_FUNC_EXIT     if(function_debug){ ds_log_func_exit();  }

#define NETMGR_LOG_IPV4_ADDR(level, prefix, ip_addr)                        \
        DS_INET4_NTOP(level, prefix, ((unsigned char *)&ip_addr))

#define NETMGR_LOG_IPV6_ADDR(level, prefix, ip_addr)                        \
        DS_INET6_NTOP(level, prefix, ((unsigned char *)&ip_addr))

#if (!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))

#define NETMGR_NL_REPORT_ADDR( level, prefix, addr )         \
        if( AF_INET6 == (addr).ss_family ) {                 \
          NETMGR_LOG_IPV6_ADDR( level, prefix, addr.__data); \
        } else {                                             \
          NETMGR_LOG_IPV4_ADDR( level, prefix, addr.__data); \
        }

#else/*(!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))*/

#define NETMGR_NL_REPORT_ADDR( level, prefix, addr )               \
        if( AF_INET6 == (addr).ss_family ) {                       \
          NETMGR_LOG_IPV6_ADDR( level, prefix, addr.__ss_padding); \
        } else {                                                   \
          NETMGR_LOG_IPV4_ADDR( level, prefix, addr.__ss_padding); \
        }

#endif/*(!defined(NETMGR_OFFTARGET) && defined(FEATURE_DS_LINUX_ANDROID))*/

#define IPTABLES_OPTS_MAX_LENGTH (15)

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef NETMGR_TEST
/*===========================================================================
  FUNCTION  netmgr_debug_malloc
===========================================================================*/
/*!
@brief
  Debug wrapper for malloc

@return
  void* - Pointer to heap memeory allocation

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void * netmgr_debug_malloc( size_t memsize );

/*===========================================================================
  FUNCTION  netmgr_debug_free
===========================================================================*/
/*!
@brief
  Debug wrapper for free

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_debug_free( void* ptr );
#endif /* NETMGR_TEST */

/*===========================================================================
  FUNCTION  netmgr_daemonize
===========================================================================*/
/*!
@brief
 Performs typical tasks required to run a program as a daemon process.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Original program will exit and a child is forked which will continue
      execution as the daemon.
*/
/*=========================================================================*/
void netmgr_daemonize (void);


#ifdef FEATURE_DATA_LOG_QXDM
/*=========================================================================
  FUNCTION:  netmgr_format_log_msg

===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void netmgr_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
);
#endif

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_util_convert_ip_addr_to_str
===========================================================================*/
/*!
@brief
  Converts the given Netmgr IP address type to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_ip_addr_to_str
(
  netmgr_ip_address_t  *ip,
  unsigned int         prefix_len,
  char                 *buf,
  unsigned int         buf_len
);

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ip_addr_to_str
===========================================================================*/
/*!
@brief
  Converts the given QMI IP address type to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ip_addr_to_str
(
  wds_ip_addr_type_v01 *ip,
  char                 *buf,
  unsigned int         buf_len
);

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ip_addr_range_to_str_range
===========================================================================*/
/*!
@brief
  Given QMI address type range converts to a string with address mask.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ip_addr_range_to_str_range
(
  wds_ip_addr_type_v01 *start_ip,
  wds_ip_addr_type_v01 *end_ip,
  char                 *buf,
  unsigned int         buf_len
);

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ipsec_key_to_str
===========================================================================*/
/*!
@brief
  Converts the given IPSec hash/crypto key to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ipsec_key_to_str
(
  uint8_t       *key,
  uint32_t      key_len,
  char          *buf,
  unsigned int  buf_len
);

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_proto_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec protocol

@return
  Pointer to the IPSec proto string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_proto_str
(
  wds_ipsec_sa_protocol_enum_v01  proto
);

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_algo_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec algorithm

@return
  Pointer to the IPSec algo string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_algo_str
(
  netmgr_ipsec_algo_type                       type,
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config
);

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_mode_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec encapsulation mode

@return
  Pointer to the IPSec mode string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_mode_str
(
  wds_ipsec_sa_encapsulate_enum_v01  mode
);

/*===========================================================================
  FUNCTION  netmgr_util_circ_list_init
===========================================================================*/
/*!
@brief
  Initializes the given circular list

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_circ_list_init
(
  netmgr_util_circ_list_type  *clist
);

/*===========================================================================
  FUNCTION  netmgr_util_circ_list_destroy
===========================================================================*/
/*!
@brief
  Frees any remaining data on the list before initializing it

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_circ_list_destroy
(
  netmgr_util_circ_list_type  *clist
);

/*===========================================================================
  FUNCTION  netmgr_util_enq_circ_list
===========================================================================*/
/*!
@brief
  Enqueues data into the circular list

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_enq_circ_list
(
  netmgr_util_circ_list_type  *clist,
  void                        *data
);

/*===========================================================================
  FUNCTION  netmgr_util_deq_circ_list
===========================================================================*/
/*!
@brief
  Dequeues data from the circular list and returns it via the out pointer

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_deq_circ_list
(
  netmgr_util_circ_list_type  *clist,
  void                        **data
);
#endif /* FEATURE_DATA_IWLAN */

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
/*===========================================================================
  FUNCTION  netmgr_util_wakelock_acquire
===========================================================================*/
/*!
@brief
  Acquires the netmgr wakelock.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_acquire();

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_release();

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_force_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock forcefully if multiple wl refs are held.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_force_release();

#elif defined(NETMGR_OFFTARGET)

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_force_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock forcefully if multiple wl refs are held.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_force_release();

#endif

#endif /* __NETMGR_UTIL_H__ */
