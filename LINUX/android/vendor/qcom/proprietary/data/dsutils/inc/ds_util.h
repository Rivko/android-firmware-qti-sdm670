/******************************************************************************

                        D S _ U T I L . H

******************************************************************************/

/******************************************************************************

  @file    ds_util.h
  @brief   Data Services Utility Functions Header File

  DESCRIPTION
  Header file for DS utility functions.

  ---------------------------------------------------------------------------
  Copyright (c) 2008, 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#ifndef __DS_UTIL_H__
#define __DS_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "comdef.h"

#ifdef FEATURE_DATA_LOG_ADB
#include <utils/Log.h>
#include "common_log.h"
#endif

#ifdef FEATURE_DATA_LOG_QXDM
#include <msg.h>
#include <msgcfg.h>
#undef LOG_TAG
#include <diag_lsm.h>
#include <log.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define DS_SUCCESS 0
#define DS_FAILURE 1


/*---------------------------------------------------------------------------
   Some general utility macros.
---------------------------------------------------------------------------*/
#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif

/*---------------------------------------------------------------------------
   Log file pointer. All log messages are written to this file.
   This variable is not meant to be used directly by clients.
---------------------------------------------------------------------------*/
extern FILE * ds_logFp;

#define ds_abort()  abort()

#define ds_assert(a)                                            \
        if (!(a)) {                                             \
            fprintf(stderr, "%s, %d: assertion (a) failed!",    \
                    __FILE__,                                   \
                    __LINE__);                                  \
            ds_abort();                                         \
        }

#ifdef FEATURE_DATA_LINUX_LE
#undef ASSERT
#define ASSERT(ex) ds_assert(ex)
#endif
/*---------------------------------------------------------------------------
   Macro for obtaining the cardinality of an array
---------------------------------------------------------------------------*/
#define ds_arrsize(a) (sizeof(a)/sizeof(a[0]))

#define DS_LOG_MASK_QXDM 0x01
#define DS_LOG_MASK_ADB 0x02
#define DS_LOG_MASK_STDOUT 0x04

/*---------------------------------------------------------------------------
   Type representing program's runtime debug level
---------------------------------------------------------------------------*/
typedef enum {
    DS_DBG_LEVEL_MIN    = 0,
    DS_DBG_LEVEL_LOW    = 0,
    DS_DBG_LEVEL_MEDIUM = 1,
    DS_DBG_LEVEL_HIGH   = 2,
    DS_DBG_LEVEL_ERROR  = 3,
    DS_DBG_LEVEL_DFLT   = DS_DBG_LEVEL_ERROR,
    DS_DBG_LEVEL_MAX    = 3
} ds_dbg_level_t;

/*---------------------------------------------------------------------------
   Different target types
---------------------------------------------------------------------------*/
typedef enum {
    DS_TARGET_INVALID = -1,
    DS_TARGET_MIN = 0,
    DS_TARGET_UNDEFINED = DS_TARGET_MIN,
    DS_TARGET_MSM,
    DS_TARGET_MSM8994,
    DS_TARGET_APQ,
    DS_TARGET_SVLTE1,
    DS_TARGET_SVLTE2,
    DS_TARGET_CSFB,
    DS_TARGET_SGLTE,
    DS_TARGET_SGLTE2,
    DS_TARGET_DSDA,
    DS_TARGET_DSDA2,
    DS_TARGET_DSDA3,
    DS_TARGET_MDM,
    DS_TARGET_FUSION4_5_PCIE,
    DS_TARGET_LE_MDM9X35,
    DS_TARGET_LE_MDM9X25,
    DS_TARGET_LE_MDM9X15,
    DS_TARGET_LE_LEGACY,
    DS_TARGET_DPM_2_0,
    DS_TARGET_JOLOKIA,
    DS_TARGET_MSM8992,
    DS_TARGET_LE_TESLA,
    DS_TARGET_TABASCO,
    DS_TARGET_MSM8996,
    DS_TARGET_FUSION_8084_9X45,
    DS_TARGET_ELDARION,
    DS_TARGET_JACALA,
    DS_TARGET_LE_ATLAS,
    DS_TARGET_FUSION_8096_9X55,
    DS_TARGET_LYKAN,
    DS_TARGET_FEERO,
    DS_TARGET_MSM8998,
    DS_TARGET_FEERO6,
    DS_TARGET_FEEROLITE6,
    DS_TARGET_SDM845,
    DS_TARGET_MSMNILE,
    DS_TARGET_STARLORD,
    DS_TARGET_STARHAWK,
    DS_TARGET_WARLOCK,
    DS_TARGET_FUSION_8998_SDX50_SA,
    DS_TARGET_FUSION_SDM845_SDX24,
    DS_TARGET_MAX,
    DS_TARGET_FORCE_32_BIT = 0x7FFFFFFF
} ds_target_t;

#define DS_SOCINFO_SOC_ID_FILE_PATH "/sys/devices/soc0/soc_id"
typedef enum{
    DS_TARGET_SOC_ID_INVALID = 0,
    DS_TARGET_SOC_ID_MDM9240 = 234,
    DS_TARGET_SOC_ID_MDM9340 = 235,
    DS_TARGET_SOC_ID_MDM9640 = 236,
    DS_TARGET_SOC_ID_MDM9245 = 237,
    DS_TARGET_SOC_ID_MDM9645 = 238,
    DS_TARGET_SOC_ID_MSM8909 = 245,
    DS_TARGET_SOC_ID_MSM8992 = 251,
    DS_TARGET_SOC_ID_MSM8996 = 246,
    DS_TARGET_SOC_ID_MSM8209 = 258,
    DS_TARGET_SOC_ID_MSM8208 = 259,
    DS_TARGET_SOC_ID_MDM9609 = 262,
    DS_TARGET_SOC_ID_MSM8929 = 268,
    DS_TARGET_SOC_ID_MSM8629 = 269,
    DS_TARGET_SOC_ID_MSM8229 = 270,
    DS_TARGET_SOC_ID_MSM8609 = 275,
    DS_TARGET_SOC_ID_MSM8952 = 264,
    DS_TARGET_SOC_ID_MSM8956 = 266,
    DS_TARGET_SOC_ID_MSM8976 = 278,
    DS_TARGET_SOC_ID_MDM9650 = 279,
    DS_TARGET_SOC_ID_MDM9250 = 283,
    DS_TARGET_SOC_ID_MDM9255 = 284,
    DS_TARGET_SOC_ID_MDM9350 = 285,
    DS_TARGET_SOC_ID_MDM9655 = 286,
    DS_TARGET_SOC_ID_MDM9607 = 290,
    DS_TARGET_SOC_ID_MDM8207 = 296,
    DS_TARGET_SOC_ID_MDM9207 = 297,
    DS_TARGET_SOC_ID_MDM9307 = 298,
    DS_TARGET_SOC_ID_MDM9628 = 299,
    DS_TARGET_SOC_ID_MSM8953 = 293,
    DS_TARGET_SOC_ID_MSM8937 = 294,
    DS_TARGET_SOC_ID_MSMGOLD_1 = 303,
    DS_TARGET_SOC_ID_MSMGOLD_2 = 308,
    DS_TARGET_SOC_ID_MSMGOLD_3 = 309,
    DS_TARGET_SOC_ID_MSM8998 = 292,
    DS_TARGET_SOC_ID_MSMFEERO6 = 313,
    DS_TARGET_SOC_ID_MSMFEEROLITE6 = 320,
    DS_TARGET_SOC_ID_SDMSTARLORD = 317,
    DS_TARGET_SOC_ID_SDMSTARHAWK = 318,
    DS_TARGET_SOC_ID_MSM8996L = 302,
    DS_TARGET_SOC_ID_MSM8996_PRO = 305,
    DS_TARGET_SOC_ID_MSM8996_AU  = 310,
    DS_TARGET_SOC_ID_MSM8996_SGAU = 315,
    DS_TARGET_SOC_ID_SDM845 = 321,
    DS_TARGET_SOC_ID_MSMNILE = 339,
    DS_TARGET_SOC_ID_JACALA_LITE = 338,
    DS_TARGET_SOC_ID_SDMWARLOCK = 336,
    DS_TARGET_SOC_ID_SDM710 = 360,
    DS_TARGET_SOC_ID_FORCE_32_BIT = 0xFFFFFFFF,
}ds_target_soc_id_t;

/*---------------------------------------------------------------------------
   Different end point types
---------------------------------------------------------------------------*/
typedef enum {
    DS_EP_TYPE_INVALID  = -1,
    DS_EP_TYPE_MIN      = 0,
    DS_EP_TYPE_HSIC     = 1,
    DS_EP_TYPE_HSUSB    = 2,
    DS_EP_TYPE_PCIE     = 3,
    DS_EP_TYPE_EMBEDDED = 4,
    DS_EP_TYPE_BAM_DMUX = 5,
    DS_EP_TYPE_MAX,
    DS_EP_TYPE_FORCE_32_BIT = 0x7FFFFFFF
} ds_ep_type_t;

typedef enum {
    DS_EP_ID_INVALID  = -1,
    DS_EP_ID_BAM_DMUX =  0,
    DS_EP_ID_EMBEDDED =  1,
    DS_EP_ID_PCIE     =  4,
    DS_EP_ID_MAX,
    DS_EP_ID_FORCE_32_BIT = 0x7FFFFFFF
} ds_ep_id_t;
/*---------------------------------------------------------------------------
   Type representing program's logging mode
---------------------------------------------------------------------------*/
typedef enum {
    DS_LOG_MODE_MIN     = 0,
    DS_LOG_MODE_DFLT    = 0,
    DS_LOG_MODE_FPRINTF = DS_LOG_MODE_DFLT,
    DS_LOG_MODE_SYSLOG  = 1,
    DS_LOG_MODE_MAX     = 1
} ds_log_mode_t;

/*---------------------------------------------------------------------------
   Type representing program's logging configuration
---------------------------------------------------------------------------*/
typedef struct {
    ds_dbg_level_t threshold; /* level at which to print */
    ds_log_mode_t  mode;      /* log method to use */
} ds_log_cfg_t;

/*---------------------------------------------------------------------------
   Macro to get log file ptr
---------------------------------------------------------------------------*/
#define ds_get_logfp() (ds_logFp)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at specified log level
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LOG_SYSLOG

#define ds_log(a, ...) ds_log_write(a, __LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at default log level
---------------------------------------------------------------------------*/
#define ds_log_dflt(...) ds_log_write(DS_DBG_LEVEL_DFLT, __LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at "error" log level
---------------------------------------------------------------------------*/
#define ds_log_err(...) ds_log_write(DS_DBG_LEVEL_ERROR, __LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at "high" log level
---------------------------------------------------------------------------*/
#define ds_log_high(...) ds_log_write(DS_DBG_LEVEL_HIGH,__LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at "med" log level
---------------------------------------------------------------------------*/
#define ds_log_med(...) ds_log_write(DS_DBG_LEVEL_MEDIUM,__LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at "low" log level
---------------------------------------------------------------------------*/
#define ds_log_low(...) ds_log_write(DS_DBG_LEVEL_LOW, __LINE__, __VA_ARGS__)

/*---------------------------------------------------------------------------
   Helper macro for logging formatted string at "error" log level, followed
   by the system error message (as obtained using std library's strerror())
---------------------------------------------------------------------------*/
#define ds_log_sys_err(a) \
        ds_log_write(DS_DBG_LEVEL_ERROR,__LINE__, a" (%d)%s", errno, strerror(errno))

#endif /* FEATURE_DATA_LOG_SYSLOG */

#ifndef DS_LOG_TAG
#define DS_LOG_TAG "QC-DS-LIB"
#undef  LOG_TAG
#define LOG_TAG DS_LOG_TAG
#endif

#ifdef FEATURE_DATA_LOG_OFFTARGET
#include "tf_log.h"

#define ds_log          TF_MSG
#define ds_log_dflt     TF_MSG
#define ds_log_err      TF_MSG_ERROR
#define ds_log_high     TF_MSG_HIGH
#define ds_log_med      TF_MSG_MED
#define ds_log_low      TF_MSG_INFO
#define ds_log_sys_err  TF_MSG_ERROR

#elif defined(FEATURE_DATA_LOG_QXDM) && defined(FEATURE_DATA_LOG_ADB)

  extern int ds_log_mask;
      /* Maximum length of log message */
      #define DS_MAX_DIAG_LOG_MSG_SIZE      512

      /* Log message to Diag */
      #define DS_LOG_MSG_DIAG( lvl, ... )                                              \
      do{                                                                              \
          if(ds_log_mask & DS_LOG_MASK_QXDM)                                           \
         {                                                                             \
           char buf[ DS_MAX_DIAG_LOG_MSG_SIZE ];                                       \
                                                                                       \
           /* Format message for logging */                                            \
           ds_format_log_msg( buf, DS_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__ );            \
                                                                                       \
           /* Log message to Diag */                                                   \
           MSG_SPRINTF_1( MSG_SSID_LINUX_DATA, lvl, "%s", buf );                       \
         }                                                                             \
     }while (0);

  #define DATA_LOGD(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGD(__VA_ARGS__);\
     }

  #define DATA_LOGE(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGE(__VA_ARGS__);\
     }

  #define DATA_LOGI(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGI(__VA_ARGS__);\
     }

      #define ds_log(...)                                    \
        do {                                                 \
          DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__);      \
          DATA_LOGI(__VA_ARGS__);                                  \
        }while (0)

      #define ds_log_dflt(...)                                   \
        do {                                                      \
          DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__);          \
          DATA_LOGE(__VA_ARGS__);                                       \
        }while (0)

      #define ds_log_err(...)                                       \
        do {                                                        \
          DS_LOG_MSG_DIAG(MSG_LEGACY_ERROR, __VA_ARGS__);            \
          DATA_LOGE(__VA_ARGS__);                                         \
        }while (0)

      #define ds_log_high(...)                                      \
        do {                                                        \
          DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__);            \
          DATA_LOGI(__VA_ARGS__);                                        \
        }while (0)


      #define ds_log_med(...)                                       \
        do {                                                        \
          DS_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__);              \
          DATA_LOGI(__VA_ARGS__);                                          \
        }while (0)

      #define ds_log_low(...)                                     \
        do {                                                        \
          DS_LOG_MSG_DIAG(MSG_LEGACY_LOW, __VA_ARGS__);              \
          DATA_LOGD(__VA_ARGS__);                                          \
        }while (0)

      #define ds_log_sys_err(a)                                                       \
        do {                                                                          \
          DS_LOG_MSG_DIAG(MSG_LEGACY_ERROR,  a " (%d)%s", errno, strerror(errno));     \
          DATA_LOGE(a " (%d)%s", errno, strerror(errno) );                                  \
        }while (0)


#elif defined(FEATURE_DATA_LOG_ADB)

  #define ds_log(...) LOGI(__VA_ARGS__)

  #define ds_log_dflt(...) LOGE(__VA_ARGS__)

  #define ds_log_err(...) LOGE(__VA_ARGS__)

  #define ds_log_high(...) LOGI(__VA_ARGS__)

  #define ds_log_med(...) LOGI(__VA_ARGS__)

  #define ds_log_low(...) LOGD(__VA_ARGS__)

  #define ds_log_sys_err(a) LOGE(a " (%d)%s", errno, strerror(errno) )

#elif defined(FEATURE_DATA_LOG_QXDM)

  /* Maximum length of log message */
  #define DS_MAX_DIAG_LOG_MSG_SIZE      512

  /* Log message to Diag */
  #define DS_LOG_MSG_DIAG( lvl, ... )                                              \
    {                                                                              \
      char buf[ DS_MAX_DIAG_LOG_MSG_SIZE ];                                        \
                                                                                   \
      /* Format message for logging */                                             \
      ds_format_log_msg( buf, DS_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__ );             \
                                                                                   \
      /* Log message to Diag */                                                    \
      MSG_SPRINTF_1( MSG_SSID_LINUX_DATA, lvl, "%s", buf );                        \
    }

  #define ds_log(...) DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

  #define ds_log_dflt(...) DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

  #define ds_log_err(...) DS_LOG_MSG_DIAG(MSG_LEGACY_ERROR, __VA_ARGS__)

  #define ds_log_high(...) DS_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

  #define ds_log_med(...) DS_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__)

  #define ds_log_low(...) DS_LOG_MSG_DIAG(MSG_LEGACY_LOW, __VA_ARGS__)

  #define ds_log_sys_err(a) DS_LOG_MSG_DIAG(MSG_LEGACY_ERROR,  a " (%d)%s", errno, strerror(errno))

#endif /* FEATURE_DATA_LOG_QXDM */

/*---------------------------------------------------------------------------
   Default message macros
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LOG_STDERR

#define ds_log(...) \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_dflt(...) \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_err(...) \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_high(...)    \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_med(...) \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_low(...)     \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define ds_log_sys_err(a) \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, a " (%d)%s\n", errno, strerror(errno))

#endif /* FEATURE_DATA_LOG_STDERR */

/*---------------------------------------------------------------------------
   Helper macro for logging function entry at low log level
---------------------------------------------------------------------------*/
#define ds_log_func_entry()        \
    ds_log_low                     \
    (                              \
        "Entering function %s\n",  \
        __FUNCTION__               \
    )

/*---------------------------------------------------------------------------
   Helper macro for logging function exit at low log level
---------------------------------------------------------------------------*/
#define ds_log_func_exit()         \
    ds_log_low                     \
    (                              \
        "Exiting function %s\n",   \
        __FUNCTION__               \
    )

#define DS_INET4_NTOP(level,prefix,data)                                             \
  ds_log_##level(prefix "IPv4 addr [%d.%d.%d.%d]",                                    \
               data[0], data[1], data[2], data[3])
#define DS_INET6_NTOP(level,prefix,data)                                             \
  ds_log_##level(prefix "IPv6 addr [%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:"             \
                      "%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x]",                         \
               data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],       \
               data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15])


#define DS_LOG_IPV4_ADDR(level, prefix, ip_addr)                            \
        DS_INET4_NTOP(level, prefix, (unsigned char*)&ip_addr)

#define DS_LOG_IPV6_ADDR(level, prefix, ip_addr)                            \
        DS_INET6_NTOP(level, prefix, (unsigned char*)&ip_addr)

#define DS_LOG_MULTICAST_LOW(fmt, ...) \
  ds_log_multicast(DS_DBG_LEVEL_LOW, fmt, __VA_ARGS__)
#define DS_LOG_MULTICAST_MED(fmt, ...) \
  ds_log_multicast(DS_DBG_LEVEL_MEDIUM, fmt, __VA_ARGS__)
#define DS_LOG_MULTICAST_HIGH(fmt, ...) \
  ds_log_multicast(DS_DBG_LEVEL_HIGH, fmt, __VA_ARGS__)
#define DS_LOG_MULTICAST_ERR(fmt, ...) \
  ds_log_multicast(DS_DBG_LEVEL_ERROR, fmt, __VA_ARGS__)

#define MAC_HEX_STRING "0123456789abcdefABCDEF" /*MAC hex check*/
#define MAC_NULL_STRING "00:00:00:00:00:00" /*MAC Null String*/
#define MAC_ADDR_LEN_HEX 6
#define MAC_ADDR_NUM_CHARS 18

#define DS_UTIL_INVALID_GID (-1)

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  ds_log_init
===========================================================================*/
/*!
@brief
  Initializes logging to use the specified log file.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void ds_log_init(FILE * logfp);

/*===========================================================================
  FUNCTION  ds_log_multicast_init
===========================================================================*/
/*!
@brief
  Initializes logging to use Android property persist.net.logmask to enable
  logging for various output streams. This function will read the property
  to set logmask bits.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - logmask bits are updated
*/
/*=========================================================================*/
void ds_log_multicast_init(void);

/*=========================================================================
  FUNCTION:  ds_format_log_msg
===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void ds_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
);

/*===========================================================================
  FUNCTION  ds_log_multicast
===========================================================================*/
/*!
@brief
  Log to various output streams based on the logmask. If n bits are set
  in the mask, enable corresponding streams for logging, thus the messages
  should be sent to n different output streams

  Example: if bit for ADB and STDOUT are set in the logmask, the log
  messages are sent to ADB as well as STDOUT.

@return
  none

@note

  - Dependencies
    - log bit mask must be set at power up

  - Side Effects
    - log messages are sent to one or more output streams
*/
/*=========================================================================*/
void ds_log_multicast(int lvl, char * fmt, ...);

/*===========================================================================
  FUNCTION  ds_log_write
===========================================================================*/
/*!
@brief
  Log printf-style formatted string using specified debug level.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
ds_log_write
(
    ds_dbg_level_t dbglvl,
    int ln,
    const char * fmt,
    ...
);

/*===========================================================================
  FUNCTION  ds_atoi
===========================================================================*/
/*!
@brief
  since stdlib atoi and strtol can't distinguish between "0" and "invalid
  numeric string", and returns 0 in both the cases, we need our own
  version of atoi.

@return
  int - numeric value of string (>=0) on success, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ds_atoi (const char * str);


/*===========================================================================
  FUNCTION  ds_malloc
===========================================================================*/
/*!
@brief
  A general purpose, reentrant memory allocator.

@return
  void * - pointer to memory block

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void * ds_malloc (size_t size);

/*===========================================================================
  FUNCTION  ds_free
===========================================================================*/
/*!
@brief
 Deallocates memory previously allocated using ds_malloc(). This is a
 reentrant function.

@return
  void

@note

  - Dependencies
    - Given memory block must have been allocated using dsc_malloc().

  - Side Effects
    - None
*/
/*=========================================================================*/
void   ds_free   (void * ptr);

/*===========================================================================
  FUNCTION  ds_system_call
===========================================================================*/
/*!
@brief
  Execute a shell command.

@return
  int - numeric value 0 on success, -1 otherwise

@note
  - Dependencies
    - None

  - Side Effects
   - None
*/
/*=========================================================================*/
int  ds_system_call
(
  const char *command,
  unsigned int cmdlen
);

/*===========================================================================
  FUNCTION  ds_system_call2
===========================================================================*/
/*!
@brief
  Execute a shell command with message logging control capability.

@return
  int - numeric value 0 on success, -1 otherwise

@note
  - Dependencies
    - None

  - Side Effects
   - None
*/
/*=========================================================================*/
int ds_system_call2
(
  const char *command,
  unsigned int cmdlen,
  boolean logmsg
) __attribute__((deprecated("Use ds_exec() instead")));

/*===========================================================================
  FUNCTION  ds_system_call3
===========================================================================*/
/*!
@brief
  Execute a shell command with message logging control capability

@param
command        - The command string to execute
cmdlen         - The length of the command string
cmd_result     - The return status string
cmd_result_len - Length of return status string

@return
  int - numeric value 0 on success, -1 otherwise

@note
  - Dependencies
    - None

  - Side Effects
   - None
*/
/*=========================================================================*/
int ds_system_call3
(
  const char    *command,
  unsigned int  cmdlen,
  char          *cmd_result,
  unsigned int  cmd_result_len,
  boolean       logmsg
) __attribute__((deprecated("Use ds_exec() instead")));

/*===========================================================================
  FUNCTION  ds_change_user_cap
===========================================================================*/
/*!
@brief
  Changes the uid/gid and sets the capabilities. uid is a system user id,
  gid is a system group id.  Capabilities should be passed as per
  requirement of capset system call.

@return
  int - numeric value 0 on success, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
ds_change_user_cap
(
  int uid,
  int gid,
  uint64_t caps
);

/*===========================================================================
  FUNCTION  ds_daemonize
===========================================================================*/
/*!
@brief
 Performs typical tasks required to run a program as a daemon process.

@return
  0 on Success, -1 otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - Original program will exit and a child is forked which will continue
      execution as the daemon.
*/
/*=========================================================================*/
int ds_daemonize (void);

/*===========================================================================
  FUNCTION  ds_get_num_bits_set_count
===========================================================================*/
/*!
@brief
 This function returns the count of bits that are set (1) in the given input
 parameter x

@return
  Count of bits set

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ds_get_num_bits_set_count
(
  unsigned int x
);

/*===========================================================================
  FUNCTION  ds_hex_to_dec
===========================================================================*/
/*!
@brief
 Read a char and returns the decimal value.

@return
  int value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ds_hex_to_dec(char ch);

/*===========================================================================
  FUNCTION  ds_get_target_offtarget()
===========================================================================*/
/*!
@brief
  Gets the current target name on an off-target platform based on
  env variable DS_TARGET

@details
  Env names are same as ds_target_t enumeration.
  DS_TARGET_MSM
  DS_TARGET_APQ
  DS_TARGET_SVLTE1
  DS_TARGET_SVLTE2
  DS_TARGET_CSFB
  DS_TARGET_SGLTE
  DS_TARGET_SGLTE2
  DS_TARGET_DSDA
  DS_TARGET_DSDA2
  DS_TARGET_DSDA3
  DS_TARGET_MDM
  DS_TARGET_FUSION4_5_PCIE
  DS_TARGET_LE_MDM9X35
  DS_TARGET_LE_MDM9X25
  DS_TARGET_LE_MDM9X15
  DS_TARGET_LE_LEGACY

@return Enum containing target name.
*/
/*=========================================================================*/
#define DS_TARGET_ENV "DS_TARGET"
ds_target_t ds_get_target_offtarget();

/*===========================================================================
  FUNCTION  ds_set_target_offtarget()
===========================================================================*/
/*!
@brief
  Set DS_TARGET variable programatically.

@arg Enum containing target type. (ds_target_t)
*/
/*=========================================================================*/
void ds_set_target_offtarget(ds_target_t target);

/*===========================================================================
  FUNCTION  ds_get_target()
===========================================================================*/
/*!
@brief
 Gets the current target name

@return Enum containing target name.

@details
  Determines the target-type by using this information:
  - Featurization to identify LE vs Android targets.
  - For LE targets
    - Uses further featurization to figure out MDM9x35 vs MDM9x25.
  - For Android targets
    - Uses ro.baseband to identify msm, apq, svlte1, svlte2, csfb, sglte, sglte2, dsda, dsda2, dsda3
    - Uses ro.baseband (mdm/mdm2) and ESOC to identify  fusion4, fusion4_5_pcie, fusion4_5_hsic
*/
/*=========================================================================*/
ds_target_t ds_get_target(void);

/*===========================================================================
  FUNCTION  ds_get_target_str()
===========================================================================*/
/*!
@brief
 Gets the current target name string.

@return
 Const char pointer to string name.
*/
/*=========================================================================*/
const char *ds_get_target_str(ds_target_t target);

/*===========================================================================
  FUNCTION ds_mac_addr_pton
  ===========================================================================
  @brief
  This function converts the MAC address from a presentation (string) format
  to a network format (Hex value).

  @input:
  char pointer pointing to MAC address in string format ,
  uint8 pointer which will store resultant MAC address in hex

  @return:
  boolean

  @dependencies
  usr to provide input

  @sideefects
  None
  =========================================================================*/
boolean ds_mac_addr_pton(const char *mac_addr_str, uint8 *mac_addr_int);

/*===========================================================================
  FUNCTION ds_mac_addr_ntop
  ===========================================================================
  @brief
  This function converts the MAC address from a network (hex) format
  to a presentation format (string).

  @input:
  uint8 pointer to MAC address in hex format ,
  char pointer which will store resultant MAC address in string format

  @return:
  void

  @dependencies
  usr to provide input

  @sideefects
  None
  =========================================================================*/
void ds_mac_addr_ntop(const uint8 *mac, char *mac_addr_str);

/*===========================================================================
  FUNCTION:  ds_get_epid
===========================================================================*/
/*!
  @brief
  This function returns the EPID/EP-TYPE information for a network
  device.

  @params[in] net_dev: network device name
  @params[out] ep_type: End point type.
  @params[out] epid: End point ID.

  @return None
*/
/*=========================================================================*/
void ds_get_epid
(
  char          *net_dev,
  ds_ep_type_t  *ep_type,
  int           *epid
);

#if (defined(FEATURE_DSUTILS_OFFTARGET) || defined(FEATURE_DATA_LINUX_LE))
pid_t gettid();
#endif

int
ds_get_proc_name
(
  int   pid,
  char  *proc_name,
  int   proc_name_size
);

/*---------------------------------------------------------------------------
   ds_exec() public definitions
---------------------------------------------------------------------------*/
#define DS_EXEC_MAX_CMD_ARGS 64

/* ----- ds_exec() options structure ---------------------------------------- */
/* All REQUIRED options are initialized by the DS_SYSCALL_DEFAULTS macro. User
 * may override these values if needed, however, they *must* be initialized to
 * sane value to get reliable operation from ds_exec
 */
struct ds_exec_opts
{
  int *rc;                    /* OPTIONAL: store the rc of the child here     */
  char *stdout;               /* OPTIONAL: Buffer to store stdout of the child*/
  int stdout_length;          /* OPTIONAL: Size of the stdout buffer          */
  char *stderr;               /* OPTIONAL: Buffer to store stderr of the child*/
  int stderr_length;          /* OPTIONAL: Size of the stderr buffer          */
  int stdout_to_stderr;       /* OPTIONAL: Redirect stdout to stderr buffer   */
  int stderr_to_stdout;       /* OPTIONAL: Redirect stderr to stdout buffer   */
  char *outfile;              /* OPTIONAL: Output file to dump the commands   */
  struct timeval tv_timeout;  /* REQUIRED: Command timeout value              */
  unsigned int log_options;   /* REQUIRED: Log mask. See DS_EXEC_LOG* values  */
  int timeout_action;         /* REQUIRED: What to do if the child times out  */
  int cmd_debug;              /* REQUIRED: Options to inject into the cmd str */
};

/* ----- ds_exec() log mask ------------------------------------------------- */
/* Log the time taken for fork() to return in both the parent and child       */
#define DS_EXEC_LOG_FORKTIME (1<<0)
/* Log the time taken for the command to execute                              */
#define DS_EXEC_LOG_EXECTIME (1<<1)
/* Log the final command string after debugging adjustments                   */
#define DS_EXEC_LOG_CMDSTR   (1<<2)
/* Log the stdout buffer. Does nothing if caller does not provide a buffer    */
#define DS_EXEC_LOG_STDOUT   (1<<3)
/* Log the stderr buffer. Does nothing if caller does not provide a buffer    */
#define DS_EXEC_LOG_STDERR   (1<<4)
/* Catch-all to enable all logs                                               */
#define DS_EXEC_LOG_ALL      0xFFFFFFFF

/* ----- ds_exec() return codes --------------------------------------------- */
#define DS_EXEC_OK           0      /* ds_exec() completed successfully          */
#define DS_EXEC_BAD_ARGS     1      /* Caller gave bad arguments                 */
#define DS_EXEC_SYSERR       2      /* A system call failed                      */
#define DS_EXEC_TIMEOUT      3      /* Command took too long to execute          */
#define DS_EXEC_NONZERORC    4      /* Command exited with a non-zero return code*/
#define DS_EXEC_WAITPIDER    5      /* waitpid() gave an unexpected return code  */
#define DS_EXEC_UNKNOWN_CMD  6      /* Command to be executed is not found       */

/* ----- ds_exec() command timeout handling options ------------------------- */
/* Kill the child process and return DS_EXEC_TIMEOUT                          */
#define DS_EXEC_TIMEOUT_KILL   0
/* Leave the child running and return DS_EXEC_TIMEOUT                         */
#define DS_EXEC_TIMEOUT_PASS   1
/* Block until child exits                                                    */
#define DS_EXEC_TIMEOUT_IGNORE 2

/* ----- ds_exec() command debugging presets -------------------------------- */
#define DS_EXEC_CMD_NODEBUG          0    /* Execute the command unmodified   */
#define DS_EXEC_CMD_STRACE           1    /* Full strace ("strace -tt -V")    */
#define DS_EXEC_CMD_STRACE_TIMEONLY  2    /* Strace system call summary only  */
#define DS_EXEC_CMD_VALGRIND         3    /* Basic valgrind                   */

/* ds_exec() location of commands on the system */
#if defined(FEATURE_DSUTILS_OFFTARGET)
  #define DS_EXEC_IPTABLES   "/sbin/iptables"
  #define DS_EXEC_IP6TABLES  "/sbin/ip6tables"
  #define DS_EXEC_NDC        "ndc"
  #define DS_EXEC_IP         "/sbin/ip"
  #define DS_EXEC_TC         "/sbin/tc"
  #define DS_EXEC_ECHO       "/bin/echo"
  #define DS_EXEC_SLEEP      "/bin/sleep"
#elif defined(FEATURE_DATA_LINUX_LE)
  #define DS_EXEC_IPTABLES   "/usr/sbin/iptables"
  #define DS_EXEC_IP6TABLES  "/usr/sbin/ip6tables"
  #define DS_EXEC_NDC        "ndc"
  #define DS_EXEC_IP         "/sbin/ip"
  #define DS_EXEC_TC         "/sbin/tc"
  #define DS_EXEC_ECHO       "/bin/echo"
  #define DS_EXEC_SLEEP      "/bin/sleep"
#else
  #define DS_EXEC_IPTABLES   "/system/bin/iptables-wrapper-1.0"
  #define DS_EXEC_IP6TABLES  "/system/bin/ip6tables-wrapper-1.0"
  #define DS_EXEC_NDC        "/system/bin/ndc-wrapper-1.0"
  #define DS_EXEC_IP         "/system/bin/ip-wrapper-1.0"
  #define DS_EXEC_TC         "/system/bin/tc-wrapper-1.0"
  #define DS_EXEC_ECHO       "/system/bin/echo"
  #define DS_EXEC_SLEEP      "/system/bin/sleep"
#endif /* defined(FEATURE_DATA_LINUX_LE) || defined(NETMGR_OFFTARGET) */

/* Macro to initialize an empty ds_exec_opts. User should always use this macro
 * and never attempt to create a struct ds_exec_opts on their own
 */
#define DS_SYSCALL_DEFAULTS(X) \
  struct ds_exec_opts X; \
  X.rc = 0; \
  X.stdout = 0; \
  X.stdout_length = 0; \
  X.stderr = 0; \
  X.stderr_length = 0; \
  X.stdout_to_stderr = 0; \
  X.stderr_to_stdout = 0; \
  X.outfile = 0; \
  X.tv_timeout.tv_sec = 2; \
  X.tv_timeout.tv_usec = 0; \
  X.log_options = 0; \
  X.timeout_action = DS_EXEC_TIMEOUT_KILL; \
  X.cmd_debug = DS_EXEC_CMD_NODEBUG;

/*===========================================================================
  FUNCTION  ds_exec
===========================================================================*/
/*!
@brief
  Execute a shell command with message logging and debug control capability

@param
command        - The null terminated command string to execute
opts           - Optional execution options. Can be NULL if no options specified

@return
  int - returns one of the DS_EXEC_* return codes.

*/
/*=========================================================================*/
int ds_exec
(
  const char *command,
  const struct ds_exec_opts *opts
);

#ifdef __cplusplus
}
#endif

#endif /* __DS_UTIL_H__ */

