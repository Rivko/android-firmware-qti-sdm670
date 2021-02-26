/*
Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*!
@file
@brief       Implementation of slim logging utilities
@ingroup slim_Log
*/

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>

#include <loc_pla.h>

#if defined(FEATURE_LOCTECH_SLIM_LOG_DIAG)
#include <msg.h>
#include <msgcfg.h>
#endif

#ifdef USE_GLIB
#include <glib.h>
#include <errno.h>
#define strlcpy g_strlcpy
#endif

#if defined(__ANDROID__)
#include <log/log.h>
#endif
#include <loc_cfg.h>

#include <slim_log_utils_i.h>

#ifndef SLIM_UNUSED
/*!
 * @brief Macro to suppress variable unused warning.
 *
 * @param x Variable literal
 */
#define SLIM_UNUSED(x) (void)(x)
#endif

#ifndef MSG_SSID_SLIM
/**
 * @brief SLIM-specific subsystem ID for DM logging
 */
#define MSG_SSID_SLIM 113
#endif

#if !defined(__ANDROID__)
//! @brief Local definition for thread-id getter
pid_t gettid(void);
#endif

#if defined(__GNUC__)
#define FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD
#endif

/* File locking: there is file locking API on Linux and Android, however
 * this API is broken on some targets.
 */
/*!
 * @def SLIM_FLOCKFILE
 * @brief Process-private file lock
 *
 * Locks a file to serialize file access between multiple threads.
 *
 * @param[in] f File to lock
 * @sa SLIM_FUNLOCKFILE
 *
 * @def SLIM_FUNLOCKFILE
 * @brief Process-private file unlock
 *
 * Unlocks a file.
 *
 * @param[in] f File to unlock
 * @sa SLIM_FLOCKFILE
 */
#ifdef FEATURE_LOCTECH_SLIM_NOFLOCK
#define SLIM_FLOCKFILE(f) pthread_mutex_lock(&z_Config.z_logMutex)
#define SLIM_FUNLOCKFILE(f) pthread_mutex_unlock(&z_Config.z_logMutex)
#else /* FEATURE_LOCTECH_SLIM_NOFLOCK */
#define SLIM_FLOCKFILE(f) flockfile(f)
#define SLIM_FUNLOCKFILE(f) funlockfile(f)
#endif /* FEATURE_LOCTECH_SLIM_NOFLOCK */

static const char * _slim_get_log_tag(const slim_LogDataT *data);
static const char * _slim_get_log_prio(const slim_LogDataT *pz_Data);

typedef struct
{
#ifdef FEATURE_LOCTECH_SLIM_NOFLOCK
  pthread_mutex_t z_logMutex;
#endif
  //! @brief Console logging level.
  slim_LogLevelT e_consoleLogLevel;
  //! @brief DIAG (DM log) logging level.
  slim_LogLevelT e_diagLogLevel;
  //! @brief OS logging level.
  slim_LogLevelT e_osLogLevel;
  //! @brief Logging flags for constructing log messages.
  uint32_t q_ExtendedLogFlags;
  //! @brief Default subsystem ID for logging
  uint32_t q_defaultDiagSSID;
} slim_LogUtilsConfigT;


static char dummy_buf[1];

static slim_LogUtilsConfigT z_Config =
{
#ifdef FEATURE_LOCTECH_SLIM_NOFLOCK
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  .z_logMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
#else
  .z_logMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER,
#endif
#endif
  .e_consoleLogLevel = SLIM_LOG_NONE,
  .e_diagLogLevel =  SLIM_LOG_NONE,
  .e_osLogLevel =  SLIM_LOG_INFO,
  .q_ExtendedLogFlags =
      SLIM_LOG_FLAG_MODULE_NAME   |
      SLIM_LOG_FLAG_FUNCTION_NAME |
      SLIM_LOG_FLAG_FILE_NAME     |
      SLIM_LOG_FLAG_THREAD_NAME,
  .q_defaultDiagSSID = MSG_SSID_SLIM
};

static char androidTag[16] = "slim";
#if defined(__ANDROID__)

static int _get_android_prio
(
  const slim_LogDataT *pzData
);
static const char * _get_android_tag
(
  const slim_LogDataT *pzData
);

//! Maps slim logging parameters to Android logging level
/**
 * \internal
 */
static int _get_android_prio
(
  const slim_LogDataT *pzData
)
{
  int prio;

  switch (pzData->level)
  {
  case SLIM_LOG_VERBOSE: prio = ANDROID_LOG_VERBOSE; break;
  case SLIM_LOG_DEBUG: prio = ANDROID_LOG_DEBUG; break;
  case SLIM_LOG_INFO: prio = ANDROID_LOG_INFO; break;
  case SLIM_LOG_WARNING: prio = ANDROID_LOG_WARN; break;
  case SLIM_LOG_ERROR: prio = ANDROID_LOG_ERROR; break;
  case SLIM_LOG_FATAL: prio = ANDROID_LOG_FATAL; break;
  case SLIM_LOG_NONE: prio = ANDROID_LOG_SILENT; break;
  default: prio = ANDROID_LOG_DEBUG; break;
  }

  return prio;
}

//! Maps slim logging parameters to Android logging tag
/**
 * \internal
 */
static const char * _get_android_tag
(
  const slim_LogDataT *pzData
)
{
  SLIM_UNUSED(pzData);

  return androidTag;
}

#endif
/**
@brief Initializes logging framework.

Method sets component logging tag and loads configuration.

@param[in] pzTag Android logging tag.
@ingroup slim_Log
*/
void slim_LogSetAndroidTag
(
  const char *pzTag
)
{
  strlcpy(androidTag, pzTag, sizeof(androidTag));

  uint32_t qLogLevel = SLIM_LOG_ERROR;
  uint32_t qLogExtended =
      SLIM_LOG_FLAG_MODULE_NAME |
      SLIM_LOG_FLAG_FUNCTION_NAME;

  uint32_t qSensorProviderCfg = 0;
  uint32_t qSensorUsageCfg = 0;
  loc_param_s_type z_sapParameterTable[] =
  {
    {"DEBUG_SLIM_LOG_EXTENDED", &qLogExtended, NULL, 'n'},
  };
  loc_param_s_type z_gpsParameterTable[] =
  {
    {"DEBUG_LEVEL", &qLogLevel, NULL, 'n'},
  };
  /* Read logging configuration and sensor provider */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
  UTIL_READ_CONF(LOC_PATH_GPS_CONF, z_gpsParameterTable);
  slim_LogSetOsLevel((slim_LogLevelT)(qLogLevel + 1));
  slim_LogSetConsoleLevel((slim_LogLevelT)(qLogLevel + 1));
  slim_LogSetExtendedFlags(qLogExtended);
}

#if defined(__ANDROID__)
//! Logs message to Android log
/**
 * \internal
 */
void _slim_LogPrintAndroid
(
  const slim_LogDataT *pz_Data,
  const char *msg
)
{
  int i_Prio;
  const char *p_Tag;
  char z_ThreadName[17];
  char z_FileName[40];
  char z_FunctionName[37];
  char z_ModuleName[30];

  if (!pz_Data || !msg || !*msg)
  {
    return;
  }

  if (z_Config.e_osLogLevel >= pz_Data->level)
  {
    uint32_t q_LogFlags = _slim_LogGetFlags(pz_Data);

    z_ThreadName[0] = 0;
    z_FileName[0] = 0;
    z_FunctionName[0] = 0;
    z_ModuleName[0] = 0;

    if (q_LogFlags & SLIM_LOG_FLAG_FILE_NAME)
    {
      const char *p_Fn = _slim_LogGetNakedFileName(pz_Data);
      size_t qLen = strlen(p_Fn);
      if (qLen > 35)
      {
        p_Fn += qLen - 35;
      }
      snprintf(z_FileName, sizeof(z_FileName), "%s:%d] ", p_Fn, pz_Data->line);
    }
    if (q_LogFlags & SLIM_LOG_FLAG_FUNCTION_NAME)
    {
      const char *p_Fn = pz_Data->func ? pz_Data->func : "?";
      if (strlen(p_Fn) > 35)
      {
        p_Fn += strlen(p_Fn) - 35;
      }
      snprintf(z_FunctionName, sizeof(z_FunctionName), "%s: ", p_Fn);
    }

    if (q_LogFlags & SLIM_LOG_FLAG_THREAD_NAME)
    {
      slim_ThreadT *pz_Thread = slim_ThreadSelf();

      snprintf(z_ThreadName, sizeof(z_ThreadName), "%s ",
          pz_Thread ? pz_Thread->z_Name : "UNBOUND");
    }

    if (q_LogFlags & SLIM_LOG_FLAG_MODULE_NAME)
    {
      const char *p_ModuleName = _slim_get_log_tag(pz_Data);
      snprintf(z_ModuleName, sizeof(z_ModuleName), "[%s] ", p_ModuleName);
    }

    i_Prio = _get_android_prio(pz_Data);
    p_Tag = _get_android_tag(pz_Data);

    __android_log_print(i_Prio, p_Tag, "%s%s%s%s%s", z_ThreadName, z_FileName,
        z_ModuleName, z_FunctionName, msg);
  }
}

#endif

//! Record system time with millisecond accuracy
/**
 * Function is used only by logging subsystem
 *
 * \return System time in milliseconds
 * \internal
 */
#if defined(__LINUX__) || defined(__ANDROID__)
slim_LogTimeT _slim_LogGetTime
(
)
{
  struct timespec tspec;

  if (clock_gettime(CLOCK_REALTIME, &tspec))
  {
    return 0;
  }

  return (uint64_t) tspec.tv_sec * 1000 + ((uint64_t) tspec.tv_nsec + 5e5) / 1e6;
}
#else
slim_LogTimeT _slim_LogGetTime
(
)
{
    return 0;
}
#endif

/*!
 * @brief Get logging flags for a given logging request.
 *
 * @param[in] pz_Data Logging request data
 *
 * @return Flags applicable to give request.
 */
uint32_t _slim_LogGetFlags
(
  const slim_LogDataT *pz_Data
)
{
  uint32_t q_Result = z_Config.q_ExtendedLogFlags;
  if (NULL != pz_Data && SLIM_LOG_MOD_SLIM_NK == pz_Data->module)
  {
    q_Result &= ~SLIM_LOG_FLAG_MODULE_NAME;
    q_Result &= ~SLIM_LOG_FLAG_FUNCTION_NAME;
  }
  return q_Result;
}

/*!
 * @brief Get file name from logging request.
 *
 * @param[in] pz_Data Logging request data
 *
 * @return File name without path delimiters.
 */
const char *_slim_LogGetNakedFileName
(
  const slim_LogDataT *pz_Data
)
{
  const char *pz_FileName = "?";
  if (NULL != pz_Data)
  {
    pz_FileName = pz_Data->fname;
    if (NULL != pz_Data->fname)
    {
      char *pz_New;
      pz_FileName = pz_Data->fname;
      pz_New = strrchr(pz_FileName, '/');
      if (NULL != pz_New)
      {
        pz_FileName = pz_New + 1;
      }
      pz_New = strrchr(pz_FileName, '\\');
      if (NULL != pz_New)
      {
        pz_FileName = pz_New + 1;
      }
    }
  }
  return pz_FileName;
}

//! Returns literal string of logging subsystem source
/**
 * \internal
 */
static const char * _slim_get_log_tag
(
  const slim_LogDataT *data
)
{
  const char *tag;

  switch (data->module)
  {
  case SLIM_LOG_MOD_HLOS: tag = "HLOS"; break;
  case SLIM_LOG_MOD_DIAG: tag = "DIAG"; break;
  case SLIM_LOG_MOD_QCSI:  tag = "QCSI"; break;
  case SLIM_LOG_MOD_QCCI:  tag = "QCCI"; break;
  case SLIM_LOG_MOD_QMIFW:  tag = "QMI FW"; break;
  case SLIM_LOG_MOD_SLIM: tag = "SLIM"; break;
  case SLIM_LOG_MOD_SLIM_NK: tag = "SLIM"; break;
  case SLIM_LOG_MOD_QS: tag = "QMISLIM"; break;
  case SLIM_LOG_MOD_NDK: tag = "NDK"; break;
  case SLIM_LOG_MOD_S1: tag = "S1"; break;
  case SLIM_LOG_MOD_MAIN: tag = "MAIN"; break;
  case SLIM_LOG_MOD_QL: tag = "QMILOC"; break;
  case SLIM_LOG_MOD_IPC: tag = "IPCSLIM"; break;
  case SLIM_LOG_MOD_VNW: tag = "VNW"; break;
  case SLIM_LOG_MOD_VNS: tag = "VNS"; break;
  default: tag = "OTHER"; break;
  }

  return tag;
}

//! Returns literal string for logging level
/**
 * \internal
 */
static const char * _slim_get_log_prio
(
  const slim_LogDataT *pz_Data
)
{
  const char *p_Prio;

  switch (pz_Data->level)
  {
  case SLIM_LOG_VERBOSE: p_Prio = "V"; break;
  case SLIM_LOG_DEBUG: p_Prio = "D"; break;
  case SLIM_LOG_INFO: p_Prio = "I"; break;
  case SLIM_LOG_WARNING: p_Prio = "W" ; break;
  case SLIM_LOG_ERROR: p_Prio = "E"; break;
  case SLIM_LOG_FATAL: p_Prio = "F"; break;
  default: p_Prio = "D"; break;
  }

  return p_Prio;
}

#if defined(FEATURE_LOCTECH_SLIM_LOG_DIAG)

static uint16_t _slim_get_diag_ssid
(
   const slim_LogDataT *data
)
{
  SLIM_UNUSED(data);
  return MSG_SSID_SLIM;
}

static uint32_t _slim_get_diag_mask
(
   const slim_LogDataT *data
)
{
  uint32_t q_Result = 0;

  if (NULL != data)
  {
    switch (data->level)
    {
    case SLIM_LOG_VERBOSE:
      q_Result = MSG_LEGACY_LOW;
      break;
    case SLIM_LOG_DEBUG:
      q_Result = MSG_LEGACY_MED;
      break;
    case SLIM_LOG_INFO:
      q_Result = MSG_LEGACY_MED;
      break;
    case SLIM_LOG_WARNING:
      q_Result = MSG_LEGACY_HIGH;
      break;
    case SLIM_LOG_ERROR:
      q_Result = MSG_LEGACY_ERROR;
      break;
    case SLIM_LOG_FATAL:
      q_Result = MSG_LEGACY_FATAL;
      break;
    default:
      break;
    }
  }
  return q_Result;
}
#endif

//! Prints message to console
/**
 * Function prints a message into \a stdout stream. The message format is:
 * timestamp priority subsystem function:line text
 *
 * \param[in] pzData  Logging parameters
 * \param[in] tTimeMs Timestamp information
 * \param[in] pMsg    Single-line message to print
 *
 * \internal
 */
void _slim_LogPrintConsole
(
   const slim_LogDataT *pzData,
   slim_LogTimeT tTimeMs,
   const char *pMsg
)
{
  const char *p_Prio;
  const char *p_ModuleName;
  char buffer[256];
  int len;
  FILE *out = stderr;
  struct tm tm;
  char z_Ctx[14] = "";
  uint32_t q_LogFlags;
  char z_ThreadName[17];
  char z_FileName[40];
  char z_FunctionName[37];
  char z_ModuleName[30];

  if (!pMsg || !*pMsg || !pzData)
  {
    return;
  }

  q_LogFlags = _slim_LogGetFlags(pzData);

  z_ThreadName[0] = 0;
  z_FileName[0] = 0;
  z_FunctionName[0] = 0;
  z_ModuleName[0] = 0;

  if (q_LogFlags & SLIM_LOG_FLAG_FILE_NAME)
  {
    const char *p_Fn = _slim_LogGetNakedFileName(pzData);
    size_t qLen = strlen(p_Fn);
    if (qLen > 35)
    {
      p_Fn += qLen - 35;
    }
    snprintf(z_FileName, sizeof(z_FileName), "%s:%d] ", p_Fn, pzData->line);
  }
  if (q_LogFlags & SLIM_LOG_FLAG_FUNCTION_NAME)
  {
    const char *p_Fn = pzData->func ? pzData->func : "?";
    if (strlen(p_Fn) > 35)
    {
      p_Fn += strlen(p_Fn) - 35;
    }
    snprintf(z_FunctionName, sizeof(z_FunctionName), "%s: ", p_Fn);
  }

  if (q_LogFlags & SLIM_LOG_FLAG_THREAD_NAME)
  {
    slim_ThreadT *pz_Thread = slim_ThreadSelf();

    snprintf(
        z_ThreadName,
        sizeof(z_ThreadName),
        "%s ",
        pz_Thread ? pz_Thread->z_Name : "UNBOUND");
  }

  if (q_LogFlags & SLIM_LOG_FLAG_MODULE_NAME)
  {
    const char *p_ModuleName = _slim_get_log_tag(pzData);
    snprintf(z_ModuleName, sizeof(z_ModuleName), "[%s] ", p_ModuleName);
  }

#if defined(__LINUX__) || defined(__ANDROID__) || defined(USE_GLIB)
  {
    time_t tt = (time_t) (tTimeMs / 1000);
#if defined(USE_GLIB)
    time(&tt);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    tTimeMs = (tv.tv_usec / 1000);
#endif

    if (!gmtime_r(&tt, &tm))
    {
      memset(&tm, 0, sizeof(tm));
    }
  }
#else
  memset(&tm, 0, sizeof(tm));
#endif
  p_Prio = _slim_get_log_prio(pzData);
  len = snprintf(
      buffer,
      sizeof(buffer),
      "%04d-%02d-%02d %02d:%02d:%02d.%03d %s %s%s%s%s",
      tm.tm_year + 1900,
      tm.tm_mon + 1,
      tm.tm_mday,
      tm.tm_hour,
      tm.tm_min,
      tm.tm_sec,
      (int) (tTimeMs % 1000),
      p_Prio,
      z_ThreadName,
      z_FileName,
      z_ModuleName,
      z_FunctionName);

  SLIM_FLOCKFILE(out);
  if (len > 0 && (size_t) len >= sizeof(buffer))
  {
    buffer[sizeof(buffer) - 1] = 0;
  }
  if (len > 0)
  {
    fputs(buffer, out);
  }
  fputs(pMsg, out);
  fputc('\n', out);
  fflush(out);
  SLIM_FUNLOCKFILE(out);
}

#if defined(FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD)
static __thread int recursive;
#endif

//! Prints a log line into OS-specific log
/**
 * Function supports Android and general console output.
 *
 * \param[in] pzData  Logging parameters
 * \param[in] tTimeMs Timestamp information
 * \param[in] pMsg    Single-line message to print
 *
 * \internal
 */
void _slim_LogPrintOsLine
(
  const slim_LogDataT *pzData,
  slim_LogTimeT tTimeMs,
  const char *pMsg
)
{
  if (!pzData || !pMsg || !*pMsg)
  {
    return;
  }

#if defined(FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD)
  if (recursive) return;
  recursive = 1;
#endif
#if defined(__ANDROID__) && defined(FEATURE_LOCTECH_SLIM_LOG_LOGCAT)
  _slim_LogPrintAndroid(pzData, pMsg);
#endif
#if defined(FEATURE_LOCTECH_SLIM_LOG_CONSOLE)
  if (z_Config.e_consoleLogLevel >= pzData->level)
  {
    _slim_LogPrintConsole(pzData, tTimeMs, pMsg);
  }
#endif
#if defined(FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD)
  recursive = 0;
#endif
}

//! Prints message to OS log and forwards it to logging handler.
/**
 * Function forwards message first to OS-specific log, and then to installed
 * logging handler, if any (DIAG).
 *
 * \param[in] pzData  Logging parameters
 * \param[in] tTimeMs Timestamp information
 * \param[in] pMsg    Single-line message to print
 *
 * \sa _slim_log_print_os_line
 * \internal
 */
void _slim_LogPrintLine
(
   const slim_LogDataT *pzData,
   slim_LogTimeT tTimeMs,
   const char *pMsg
)
{
  if (!pzData || !pMsg || !*pMsg)
  {
    return;
  }

  _slim_LogPrintOsLine(pzData, tTimeMs, pMsg);

#if defined(FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD)
  recursive = 1;
#endif
#if defined(FEATURE_LOCTECH_SLIM_LOG_DIAG)
  if (z_Config.e_diagLogLevel >= pzData->level)
  {
    msg_const_type z_msg;
    char z_FunctionName[37] = "";
    char z_ModuleName[30] = "";
    uint32_t q_LogFlags = _slim_LogGetFlags(pzData);

    memset(&z_msg, 0, sizeof(z_msg));
    z_msg.desc.line = pzData->line;
    z_msg.desc.ss_id = _slim_get_diag_ssid(pzData);
    z_msg.desc.ss_mask = _slim_get_diag_mask(pzData);
    z_msg.fname = pzData->fname;
    z_msg.fmt = "@SLIMAP %s%s%s";

    if (q_LogFlags & SLIM_LOG_FLAG_MODULE_NAME)
    {
      const char *p_ModuleName = _slim_get_log_tag(pzData);
      snprintf(z_ModuleName, sizeof(z_ModuleName), "[%s] ", p_ModuleName);
    }
    if (q_LogFlags & SLIM_LOG_FLAG_FUNCTION_NAME)
    {
      const char *p_Fn = pzData->func ? pzData->func : "?";
      if (strlen(p_Fn) > 35)
      {
        p_Fn += strlen(p_Fn) - 35;
      }
      snprintf(z_FunctionName, sizeof(z_FunctionName), "%s: ", p_Fn);
    }

    msg_sprintf(&z_msg, z_ModuleName, z_FunctionName, pMsg);
  }
#endif
#if defined(FEATURE_LOCTECH_SLIM_TLS_LOG_GUARD)
  recursive = 0;
#endif
}

static void slim_LogPrintFormat
(
   const slim_LogDataT *pzData,
   slim_LogPrintLineFnT *pfnPrintLine,
   const char *pFormat,
   va_list zArgs
)
{
  size_t qLen;
  char *pBuffer, *pPtr;
  slim_LogTimeT tNowMs;

  if (NULL == pzData || NULL == pFormat || 0 == *pFormat ||
      NULL == pfnPrintLine)
  {
    return;
  }

  tNowMs = _slim_LogGetTime();

  qLen = vsnprintf(dummy_buf, 0, pFormat, zArgs);

  if (qLen <= 0)
  {
    goto err_0;
  }

  pBuffer = slim_Malloc(qLen + 1);
  if (!pBuffer)
  {
    goto err_0;
  }

  qLen = vsnprintf(pBuffer, qLen + 1, pFormat, zArgs);

  if (qLen <= 0)
  {
    goto err_1;
  }

  pPtr = pBuffer;
  for (;;)
  {
    char *end = strchr(pPtr, '\n');

    if (end)
    {
      *end = 0;
    }

    pfnPrintLine(pzData, tNowMs, pPtr);

    if (end)
    {
      pPtr = end + 1;
    }
    else
    {
      break;
    }
  }

err_1:
  slim_Free(pBuffer);
err_0:
  ;
}
/*!
 * @brief Logs raw data using given output function.
 *
 * The function formats raw (binary) data in a form of HEX dump. The function
 * logs 16 bytes per line with hexadecimal and ASCII output.
 *
 * @param[in] pzData       Logging context.
 * @param[in] pfnPrintLine Output function.
 * @param[in] pBuffer      Data buffer to log.
 * @param[in] qSize        Amount of data to log.
 */
void slim_LogDataFormat
(
  const slim_LogDataT *pzData,
  slim_LogPrintLineFnT *pfnPrintLine,
  const void *pBuffer,
  uint32_t qSize
)
{
  char pStr[256 * 3 + 1];
  uint32_t i, j;
  const unsigned char *pSrc = pBuffer;
  slim_LogTimeT tNowMs;

  if (!pBuffer)
  {
    return;
  }

  tNowMs = _slim_LogGetTime();

  for (i = 0; i < qSize; i += 16)
  {
    int k = snprintf(pStr, sizeof(pStr), "%04X:", (int) i);
    if (k < 0)
    {
      break;
    }

    char *pPtr = pStr + k;

    for (j = i; j < qSize && j < i + 16; j++)
    {
      unsigned char hi = pSrc[j];
      unsigned char lo = hi & 0xF;
      hi >>= 4;
      switch (j & 0xF)
      {
      case 4:
      case 8:
      case 12:
        *pPtr++ = ' ';
        break;
      default:
        break;
      }
      *pPtr++ = ' ';
      *pPtr++ = hi + (hi > 9 ? 'A' - 10 : '0');
      *pPtr++ = lo + (lo > 9 ? 'A' - 10 : '0');
    }

    for (; j < i + 16; j++)
    {
      switch (j & 0xF)
      {
      case 0:
        *pPtr++ = ' ';
        /* no break */
      case 4:
      case 8:
      case 12:
        *pPtr++ = ' ';
        break;
      default:
        break;
      }
      *pPtr++ = ' ';
      *pPtr++ = '_';
      *pPtr++ = '_';
    }
    *pPtr++ = ' ';
    *pPtr++ = ' ';

    for (j = i; j < qSize && j < i + 16; j++)
    {
      if (isprint(pSrc[j]))
      {
        *pPtr++ = pSrc[j];
      }
      else
      {
        *pPtr++ = '.';
      }
    }
    for (; j < i + 16; j++)
    {
      *pPtr++ = '.';
    }
    *pPtr = 0;

    pfnPrintLine(pzData, tNowMs, pStr);
  }
}

/**
 * @brief Logs formatted messages.
 *
 * The method logs a formatted message using @a printf style formatting
 * conventions.
 *
 * @param[in] pzData    Logging context information
 * @param[in] pFormat   Formating string.
 * @param[in] ...       Additional arguments.
 *
 * @sa printf
 * @ingroup slim_Log
 */
void slim_LogOsPrintf
(
  const slim_LogDataT *pzData,
  const char *pFormat,
  ...
)
{
  va_list z_Args;
  va_start(z_Args, pFormat);
  slim_LogPrintFormat(pzData, _slim_LogPrintOsLine, pFormat, z_Args);
  va_end(z_Args);
}

/**
 * @brief Logs formatted messages.
 *
 * The method logs a formatted message using @a printf style formatting
 * conventions.
 *
 * @param[in] pzData    Logging context information
 * @param[in] pFormat   Formating string.
 * @param[in] ...       Additional arguments.
 *
 * @sa printf
 * @ingroup slim_Log
 */
void slim_LogPrintf
(
  const slim_LogDataT *pzData,
  const char *pFormat,
  ...
)
{
  va_list z_Args;
  va_start(z_Args, pFormat);
  slim_LogPrintFormat(pzData, _slim_LogPrintLine, pFormat, z_Args);
  va_end(z_Args);
}
/**
 * @brief Logs message with three ordinal arguments
 *
 * @param[in] pzData     Logging context.
 * @param[in] pFormat    Message to log with DIAG-style arguments.
 * @param[in] qArg0      First argument.
 * @param[in] qArg1      Second argument.
 * @param[in] qArg2      Third argument.
 *
 * @ingroup slim_Log
 */
void slim_Log3
(
  const slim_LogDataT *pzData,
  const char *pFormat,
  uint32_t qArg0,
  uint32_t qArg1,
  uint32_t qArg2
)
{
  slim_LogPrintf(pzData, pFormat, qArg0, qArg1, qArg2);
}

/**
 * @brief Logs message.
 *
 * @param[in] pzData    Logging context.
 * @param[in] pMessage  Message to log.
 *
 * @ingroup slim_Log
 */
void slim_LogOsPuts
(
  const slim_LogDataT *pzData,
  const char *pMessage
)
{
  slim_LogTimeT now;

  if (!pzData || !pMessage || !*pMessage)
  {
    return;
  }

  now = _slim_LogGetTime();
  _slim_LogPrintOsLine(pzData, now, pMessage);
}

/**
 * @brief Logs message
 *
 * @param[in] pzData     Logging context.
 * @param[in] pMessage   Message to log.
 *
 * @ingroup slim_Log
 */
void slim_LogPuts
(
   const slim_LogDataT *pzData,
   const char *pMessage
)
{
  slim_LogTimeT now;

  if (!pzData || !pMessage || !*pMessage)
  {
    return;
  }

  now = _slim_LogGetTime();
  _slim_LogPrintLine(pzData, now, pMessage);
}

/**
 * @brief Logs binary message
 *
 * Logs binary data in a form of hexadecimal dump with ASCII literals. The
 * data is logged in a form of 16 bytes per line.
 *
 * @param[in] pzData     Logging context
 * @param[in] pBuffer    Binary data to log
 * @param[in] qSize      Length of data to log
 *
 * @ingroup slim_Log
 */
void slim_LogData
(
  const slim_LogDataT *pzData,
  const void *pBuffer,
  uint32_t qSize
)
{
  slim_LogDataFormat(pzData, _slim_LogPrintLine, pBuffer, qSize);
}

/**
 * @brief Logs binary message
 *
 * Logs binary data in a form of hexadecimal dump with ASCII literals. The
 * data is logged in a form of 16 bytes per line.
 *
 * @param[in] pzData     Logging context
 * @param[in] pBuffer    Binary data to log
 * @param[in] qSize      Length of data to log
 *
 * @ingroup slim_Log
 */
void slim_LogOsData
(
  const slim_LogDataT *pzData,
  const void *pBuffer,
  uint32_t qSize
)
{
  slim_LogDataFormat(pzData, _slim_LogPrintLine, pBuffer, qSize);
}
/**
 * @brief Set console logging level.
 * @ingroup slim_Log
 */
void slim_LogSetConsoleLevel
(
  slim_LogLevelT e_Level
)
{
  z_Config.e_consoleLogLevel = e_Level;
}

/**
 * @brief Set Diag interface logging level.
 * @ingroup slim_Log
 */
void slim_LogSetDiagLevel
(
  slim_LogLevelT e_Level
)
{
  z_Config.e_diagLogLevel = e_Level;
}

/**
 * @brief Set OS-specific logging level.
 * @ingroup slim_Log
 */
void slim_LogSetOsLevel
(
  slim_LogLevelT e_Level
)
{
  z_Config.e_osLogLevel = e_Level;
}

/**
 * @brief Test logging level
 *
 * @return true if message is logged; false if ignored.
 * @ingroup slim_Log
 */
bool slim_LogIsLoggable
(
  slim_LogModT e_Module,
  /**< [in] SLIM logging module */
  slim_LogLevelT e_Level
  /**< [in] SLIM logging level */
)
{
  SLIM_UNUSED(e_Module);
  if (z_Config.e_consoleLogLevel >= e_Level ||
      z_Config.e_osLogLevel >= e_Level ||
      z_Config.e_diagLogLevel >= e_Level)
  {
    return true;
  }
  return false;
}
void slim_LogSetExtendedFlags
(
  uint32_t q_Flags
)
{
  z_Config.q_ExtendedLogFlags = q_Flags;
}
