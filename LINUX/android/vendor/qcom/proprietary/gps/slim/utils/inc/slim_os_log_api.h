/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef SLIM_OS_LOG_API_H_INCLUDED
#define SLIM_OS_LOG_API_H_INCLUDED
/**
 * @file
 * @brief SLIM logging API header file
 * @ingroup slim_Log
 *
 * This header file is an adaptation of QCA1530 UPOS logging API to
 * remove legacy dependencies.
 */
#include <stdint.h>
#include <stdbool.h>

//! @addtogroup slim_Log
//! @{

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def SLIM_ATTR_FN_PRINTF
 * \brief Macro for specifying printf-like format string parameter
 *
 * This macro is used as a function attribute to mark parameter as a format
 * string:
 * \code
 * void my_printf(
 *     FRM_ATTR_FRM_ARG const char *fmt,
 *     ...
 *     ) SLIM_ATTR_FN_PRINTF(1);
 * \endcode
 *
 * \param x Parameter number
 * \sa SLIM_ATTR_PRINTF
 * @ingroup slim_Log
 *
 * \def SLIM_ATTR_PRINTF
 * \brief Macro for specifying printf-like format string parameter
 *
 * This macro is used as a parameter type specifier:
 * \code
 * void my_printf(
 *     FRM_ATTR_FRM_ARG const char *fmt,
 *     ...
 *     ) SLIM_ATTR_FN_PRINTF(1);
 * \endcode
 *
 * \sa SLIM_ATTR_FN_PRINTF
 * @ingroup slim_Log
 */

/* printf-attribute section */
#if defined(__GNUC__)
#define SLIM_ATTR_FN_PRINTF(x) __attribute__((format(printf,(x),(x)+1)))
#define SLIM_ATTR_PRINTF
#elif defined(_MSC_VER)
#define SLIM_ATTR_FN_PRINTF(x)
#define SLIM_ATTR_PRINTF
#else
#define SLIM_ATTR_FN_PRINTF(x)
#define SLIM_ATTR_PRINTF
#endif

#ifndef SLIM_LOG_FILE_NAME_LEN_MAX
//! Maximum file name length limit for SLIM logging
/**
 * SLIM logging uses this constant to truncate message source file name if it
 * is longer, then this value.
 *
 * \sa SLIM_LOG
 * @ingroup slim_Log
 */
#define SLIM_LOG_FILE_NAME_LEN_MAX 128
#endif /* SLIM_LOG_FILE_NAME_LEN_MAX */

//! Supported logging levels
/**
 * The type defines general logging message severity level.
 *
 * @ingroup slim_Log
 */
typedef enum
{
  SLIM_LOG_NONE    = 0,
  /**<
   * @brief No logging
   * @details Special level used for disabling logging for particular
   * component.
   */
  SLIM_LOG_FATAL   = 1,
  /**<
   * @brief Critial error.
   * @details System can not continue activity without user intervention.
   */
  SLIM_LOG_ERROR   = 2,
  /**<
   * @brief Error message.
   * @details Subsystem has encountered an error, but can continue. User
   * intervention may be required.
   */
  SLIM_LOG_WARNING = 3,
  /**<
   * @brief Warning message.
   * @details User attention is required.
   */
  SLIM_LOG_INFO    = 4,
  /**<
   * @brief Information message.
   * @details Important information regarding system operation. No user
   * actions are required.
   */
  SLIM_LOG_DEBUG   = 5,
  /**<
   * @brief Debugging message.
   * @details Information available only when requested by user.
   */
  SLIM_LOG_VERBOSE = 6,
  /**<
   * @brief Verbose information.
   * @details Most verbose logging level. Available only on user request.
   */
} slim_LogLevelT;

//! SLIM Logging source
/**
 * This type is used for configuring per-system logging levels and
 * correct mapping of logging event source to external facilities like DIAG.
 *
 * - @ref SLIM_LOG_MOD_UNKNOWN Undefined (unknown) module
 * - @ref SLIM_LOG_MOD_SLIM    OS integration facilities
 * - @ref SLIM_LOG_MOD_DIAG    DIAG integration layer
 * - @ref SLIM_LOG_MOD_QCSI    QMI Service Interface
 * - @ref SLIM_LOG_MOD_QCCI    QMI Client Interface
 * - @ref SLIM_LOG_MOD_QMIFW   QMI Framework
 * - @ref SLIM_LOG_MOD_USER_FIRST First user-defined module code
 *
 * @sa slim_LogDataT
 * @sa SLIM_LOG
 * @ingroup slim_Log
 */
typedef uint16_t slim_LogModT;
//! @brief Unknown logging module.
#define SLIM_LOG_MOD_UNKNOWN     ((slim_LogModT)0)
//! @brief Unknown logging module.
#define SLIM_LOG_MOD_HLOS        ((slim_LogModT)1)
//! @brief DIAG logging module.
#define SLIM_LOG_MOD_DIAG        ((slim_LogModT)2)
//! @brief QMI CSI logging module.
#define SLIM_LOG_MOD_QCSI        ((slim_LogModT)3)
//! @brief QMI CCI logging module.
#define SLIM_LOG_MOD_QCCI        ((slim_LogModT)4)
//! @brief QMI framework logging module.
#define SLIM_LOG_MOD_QMIFW       ((slim_LogModT)5)
//! @brief QMI-SLIM logging module.
#define SLIM_LOG_MOD_QS          ((slim_LogModT)6)
//! @brief SLIM logging module.
#define SLIM_LOG_MOD_SLIM        ((slim_LogModT)7)
//! @brief Main logging module.
#define SLIM_LOG_MOD_MAIN        ((slim_LogModT)8)
//! @brief Android NDK Provider logging module.
#define SLIM_LOG_MOD_NDK         ((slim_LogModT)9)
//! @brief Sensor1 Provider logging module.
#define SLIM_LOG_MOD_S1          ((slim_LogModT)10)
//! @brief QMI-LOC client logging module.
#define SLIM_LOG_MOD_QL          ((slim_LogModT)11)
//! @brief IPC logging module.
#define SLIM_LOG_MOD_IPC         ((slim_LogModT)12)
//! @brief Vehicle Network Provider logging module.
#define SLIM_LOG_MOD_VNW         ((slim_LogModT)14)
//! @brief SLIM logging module (without prefix).
#define SLIM_LOG_MOD_SLIM_NK     ((slim_LogModT)15)
//! @brief Sensor HAL logging module start.
#define SLIM_LOG_MOD_HAL          ((slim_LogModT)16)
//! @brief SLIM logging module (without prefix).
#define SLIM_LOG_MOD_VNS         ((slim_LogModT)17)
//! @brief Custom user logging module start.
#define SLIM_LOG_MOD_USER_FIRST  ((slim_LogModT)100)

/*@brief SLIM extended logging flags */
//! @brief Flag for DM log logging.
#define SLIM_LOG_FLAG_DIAG          ((uint32_t)1 << 4)
//! @brief Flag for logging context file name.
#define SLIM_LOG_FLAG_FILE_NAME     ((uint32_t)1 << 3)
//! @brief Flag for logging context module name.
#define SLIM_LOG_FLAG_MODULE_NAME   ((uint32_t)1 << 2)
//! @brief Flag for logging context function name.
#define SLIM_LOG_FLAG_FUNCTION_NAME ((uint32_t)1 << 1)
//! @brief Flag for logging context thread name.
#define SLIM_LOG_FLAG_THREAD_NAME   ((uint32_t)1 << 0)

//! Structure for using with SLIM logging API
/**
 * The structure is used for transferring additional data to logging
 * subsystem. This includes logging message source code position, origin
 * subsystem and priority.
 *
 * When building with MSVC \a __LINE__ macro may expand to value reference, and
 * the value or \a #func could change between logging calls.
 *
 * @sa slim_LogPrintf
 * @sa slim_LogPuts
 *
 * @ingroup slim_Log
 */
typedef struct
{
  const char      *fname;
  /**<
   * @brief Source module file name
   */
  const char      *func;
  /**<
   * @brief Function name
   */
  int              line;
  /**<
   * @brief Source module line number
   */
  slim_LogModT     module;
  /**<
   * @brief Module identifier
   * @details Module identifier enables per-module message filtering and
   * enables additional information regarding source of the logging message.
   */
  slim_LogLevelT   level;
  /**<
   * @brief Message severity
   * @details Message severity hints logging subsystem if the message
   * shall be logged or not.
   */
} slim_LogDataT;

/**
 * @brief Set console logging level.
 * @ingroup slim_Log
 */
void slim_LogSetConsoleLevel
(
  slim_LogLevelT e_Level
);

/**
 * @brief Set Diag interface logging level.
 * @ingroup slim_Log
 */
void slim_LogSetDiagLevel
(
  slim_LogLevelT e_Level
);

/**
 * @brief Set OS-specific logging level.
 * @ingroup slim_Log
 */
void slim_LogSetOsLevel
(
  slim_LogLevelT e_Level
);

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
);

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
  SLIM_ATTR_PRINTF const char *pFormat,
  ...
) SLIM_ATTR_FN_PRINTF(2);

/**
 * @brief Logs message
 *
 * @param[in] pzData    Logging context.
 * @param[in] pMessage  Message to log.
 *
 * @ingroup slim_Log
 */
void slim_LogPuts
(
  const slim_LogDataT *pzData,
  const char *pMessage
);

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
);

/**
 * @brief Logs message with three ordinal arguments
 *
 * @param[in] pzData    Logging context.
 * @param[in] pFormat   Message to log with DIAG-style arguments.
 * @param[in] qArg0     First argument.
 * @param[in] qArg1     Second argument.
 * @param[in] qArg2     Third argument.
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
);

// Direct OS logging functions (see source files)
/**
 * @brief Logs formatted message with parameters.
 *
 * @param[in] pzData    Logging context information
 * @param[in] pFormat   Formating string.
 * @param[in] ...       Additional arguments.
 *
 * @ingroup slim_Log
 */
void slim_LogOsPrintf
(
  const slim_LogDataT *pzData,
  SLIM_ATTR_PRINTF const char *pFormat,
  ...
) SLIM_ATTR_FN_PRINTF(2);
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
);

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
);

/**
@brief Initializes logging framework.

Method sets component logging tag and loads configuration.

@param[in] pzTag Android logging tag.
@ingroup slim_Log
*/
void slim_LogSetAndroidTag
(
  const char *pzTag
);
/**
@brief Changes extended logging flags.

Extending logging flags control which information is presented to the user
with logging message.

@param[in] q_Flags Logging flags.

@ingroup slim_Log
*/
void slim_LogSetExtendedFlags
(
  uint32_t q_Flags
);

//! Initialize logging data structure
/**
 * @def SLIM_LOG_CONST
 * This macro is used internally to correctly initialize logging data
 * structure.
 *
 * The structure is allocated on const data segment and is not supposed to
 * change.
 *
 * @param[in] module Module that issues logging. Must be compile-time constant.
 * @param[in] level  Logging level. Must be compile-time constant.
 *
 * @internal
 * @ingroup slim_Log
 */
#define SLIM_LOG_CONST(module, level)                 \
  static const slim_LogDataT __slim_LogData =         \
  {                                                   \
    __FILE__ , __FUNCTION__, __LINE__, module, level  \
  };

/**
 * \def SLIM_LOG
 * \brief Macro for formatted message logging
 *
 * \a SLIM_LOG, \a SLIM_WLOG and \a SLIM_TLOG are general-purpose logging
 * macros for different locales. They format the message and direct it to
 * DIAG and other logging handlers as appropriate.
 *
 * \a SLIM_OS_LOG, \a SLIM_OS_WLOG and \a SLIM_OS_TLOG are system logging
 * macros for different locales. Ulike above, these macros forward logging
 * messages only to OS-specific logging facilities.
 *
 * \param[in] module Module identifier. Must be compile-time constant.
 * \param[in] level  Logging level. Must be compile-time constant.
 * \param[in] ...    Formatting string and additional arguments.
 *
 * \sa SLIM_LOG
 * \sa SLIM_OS_LOG
 *
 * \sa SLIM_LOG0
 * \sa SLIM_OS_LOG0
 *
 * \def SLIM_LOG0
 * \brief Macro for single-line message logging
 *
 * \a SLIM_LOG0, \a SLIM_WLOG0 and \a SLIM_TLOG0 are single-line logging macros
 * for different locale builds. They use simplified interface for processing
 * message and direct it to DIAG and other logging handlers as appropriate.
 *
 * \a SLIM_OS_LOG0, \a SLIM_OS_WLOG0 and \a SLIM_OS_TLOG are system logging
 * macros for different locales. Unlike above these macros forward logging
 * messages only to OS-specific logging facilities.
 *
 * \param[in] module Module identifier. Must be compile-time constant.
 * \param[in] level  Logging level. Must be compile-time constant.
 * \param[in] msg    Message to log.
 *
 * \sa SLIM_LOG
 * \sa SLIM_OS_LOG
 *
 * \sa SLIM_LOG0
 * \sa SLIM_OS_LOG0
 * @ingroup slim_Log
 *
 * \def SLIM_OS_LOG
 * \brief Macro for formatted message OS logging
 * \copydetails SLIM_LOG
 * @ingroup slim_Log
 *
 * \def SLIM_OS_LOG0
 * \brief Macro for single-line OS message logging
 * \copydetails SLIM_LOG0
 * @ingroup slim_Log
 *
 * \def SLIM_LOG3
 * \brief Macro for logging using DIAG parameter contract
 *
 * In DIAG logging all parameters are casted to uint32_t data type.
 * @ingroup slim_Log
 *
 * \def SLIM_LOG_DATA
 * \brief Macro for raw data logging
 *
 * @ingroup slim_Log
 *
 * \def SLIM_LOG_OS_DATA
 * \brief Macro for raw data OS logging
 *
 * \copydetails SLIM_LOG_DATA
 * @ingroup slim_Log
 *
 */
#define SLIM_LOG(module,level,...) \
    _SLIM_LOG_TEMPLATE(slim_LogPrintf,module,level,__VA_ARGS__)
#define SLIM_LOG0(module,level,msg) \
    _SLIM_LOG_TEMPLATE(slim_LogPuts,module,level,msg)
#define SLIM_LOG3(module,level,msg,a,b,c) \
    _SLIM_LOG_TEMPLATE(slim_Log3,module,level,msg,(uint32_t)(a),(uint32_t)(b),(uint32_t)(c))
#define SLIM_OS_LOG(module,level,...) \
    _SLIM_LOG_TEMPLATE(slim_LogOsPrintf,module,level,__VA_ARGS__)
#define SLIM_OS_LOG0(module,level,msg) \
    _SLIM_LOG_TEMPLATE(slim_LogOsPuts,module,level,msg)
#define SLIM_LOG_DATA(module,level,buffer,size) \
    _SLIM_LOG_TEMPLATE(slim_LogData,module,level,buffer,size)
#define SLIM_LOG_OS_DATA(module,level,buffer,size) \
    _SLIM_LOG_TEMPLATE(slim_LogOsData,module,level,buffer,size)

/**
 * \def SLIM_LOG_LOCAL_MODULE
 * \brief Macro for mapping default stdio operations to correct component
 *
 * Any module can define own substitution to macro \a SLIM_LOG_DEFAULT_MODULE
 * inside header or source file (preferred):
 * \code{.c}
 * #undef SLIM_LOG_LOCAL_MODULE
 * #define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_AME
 * \endcode
 * @ingroup slim_Log
 */
#ifndef SLIM_LOG_LOCAL_MODULE
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_SLIM
#endif

//! @brief Logging macro for verbose level.
#define SLIM_LOGV(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_VERBOSE,__VA_ARGS__)
//! @brief Logging macro for debug level.
#define SLIM_LOGD(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_DEBUG,__VA_ARGS__)
//! @brief Logging macro for info level.
#define SLIM_LOGI(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_INFO,__VA_ARGS__)
//! @brief Logging macro for warning level.
#define SLIM_LOGW(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_WARNING,__VA_ARGS__)
//! @brief Logging macro for error level.
#define SLIM_LOGE(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_ERROR,__VA_ARGS__)
//! @brief Logging macro for fatal error level.
#define SLIM_LOGF(...) SLIM_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_FATAL,__VA_ARGS__)
//! @brief OS logging macro for verbose level.
#define SLIM_OS_LOGV(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_VERBOSE,__VA_ARGS__)
//! @brief OS logging macro for debug level.
#define SLIM_OS_LOGD(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_DEBUG,__VA_ARGS__)
//! @brief OS logging macro for info level.
#define SLIM_OS_LOGI(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_INFO,__VA_ARGS__)
//! @brief OS logging macro for warning level.
#define SLIM_OS_LOGW(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_WARNING,__VA_ARGS__)
//! @brief OS logging macro for error level.
#define SLIM_OS_LOGE(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_ERROR,__VA_ARGS__)
//! @brief OS logging macro for fatal level.
#define SLIM_OS_LOGF(...) SLIM_OS_LOG(SLIM_LOG_LOCAL_MODULE,SLIM_LOG_FATAL,__VA_ARGS__)
//! @brief IF-type macro when verbose logging is enabled
#define IF_SLIM_LOGV if (slim_LogIsLoggable(SLIM_LOG_LOCAL_MODULE, SLIM_LOG_VERBOSE))
//! @brief IF-type macro when debug logging is enabled
#define IF_SLIM_LOGD if (slim_LogIsLoggable(SLIM_LOG_LOCAL_MODULE, SLIM_LOG_DEBUG))
//! @brief IF-type macro when info logging is enabled
#define IF_SLIM_LOGI if (slim_LogIsLoggable(SLIM_LOG_LOCAL_MODULE, SLIM_LOG_INFO))
//! @brief IF-type macro when warning logging is enabled
#define IF_SLIM_LOGW if (slim_LogIsLoggable(SLIM_LOG_LOCAL_MODULE, SLIM_LOG_WARNING))

//! @brief Helper macro to log function entry point
#define SLIM_LOG_ENTRY()   SLIM_LOGV("Entering function")
//! @brief Helper macro to log function exit point
#define SLIM_LOG_EXIT(a,b)
//! @brief Helper macro to log function entry point
#define SLIM_LOG_CALLFLOW() SLIM_LOGV("AFW: Entering function")

//! \internal
/**
 * @brief Helper template for defining logging operation.
 *
 * This macro defines logging context variable and logging function invocation.
 *
 * @param[in] fn     Logging function to invoke.
 * @param[in] module Logging module.
 * @param[in] level  Logging level.
 * @param[in] ...    Additional arguments for logging function.
 *
 * @ingroup slim_Log
 */
#define _SLIM_LOG_TEMPLATE(fn,module,level,...) \
  do                                            \
  {                                             \
    SLIM_LOG_CONST(module, level);              \
    fn(&__slim_LogData, __VA_ARGS__);           \
  }                                             \
  while (0)

#ifdef __cplusplus
}
#endif

//! @}

#endif /* SLIM_OS_LOG_API_H_INCLUDED */
