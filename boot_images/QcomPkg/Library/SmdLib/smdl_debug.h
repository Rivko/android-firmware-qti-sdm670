#ifndef SMDL_DEBUG_H
#define SMDL_DEBUG_H

/**
 * @file smdl_debug.h
 *
 * SMD Lite internal header for debugging messages
 *
 * SMD Lite logging is intended mostly for debugging SMD Lite itself during
 * development and integration. It uses smem_log and target-specific logging.
 * Each log entry has a particular logging level; these will be disabled or
 * enabled at compile-time based on the overall logging level, SMDL_LOGLEVEL.
 */

/*==============================================================================
     Copyright (c) 2009-2011, 2014 Qualcomm Technologies, Inc.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/14   bm      Warning cleanup
03/19/12   bm      Disable logging for RPM proc
05/20/11   bm      Disable logging for Q6FW and DSPS procs
04/21/10   tl      Updated to SMD Lite v2
04/20/09   tl      Initial release of SMD Lite
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "smem_log.h"
#include "smd_target.h"

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/** The priority of a log message */
typedef enum {
  /**
   * Debug-level log messages
   *
   * Debug log messages are verbose messages detailing the flow of function
   * calls and the inner structure of the fifos during read and write
   * operations.
   */
  SMDL_LOGLEVEL_DEBUG,

  /**
   * Info-level log messages
   *
   * Info log messages identify non-exceptional initalization, open, and close
   * operations.
   */
  SMDL_LOGLEVEL_INFO,

  /**
   * Warning-level log messages
   *
   * Warning log messages identify exceptional but recoverable error
   * conditions.
   */
  SMDL_LOGLEVEL_WARN,

  /**
   * Error-level log messages
   *
   * Error log messages identify exceptional and possibly fatal error
   * conditions.
   */
  SMDL_LOGLEVEL_ERROR
} smdl_loglevel_type;

#ifndef SMDL_LOGLEVEL
/**
 * The default logging level, if one is not already specified
 *
 * Messages below this logging level will be disabled at compile time; all
 * other messages will be printed.
 */
# define SMDL_LOGLEVEL SMDL_LOGLEVEL_INFO
/* UEFI generates warning for unsued variable, comemnted out */
//static smdl_loglevel_type smdl_loglevel;
#endif /* !SMDL_LOGLEVEL */

/** The base event for SMD Lite smem_log messages, used to uniquely identify
 * SMD Lite log messages in smem_log. */
#define SMEM_LOG_SMDLITE_EVENT_BASE (SMEM_LOG_SMEM_EVENT_BASE + 0x8000)

/*=========================================================================== 
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/**
 * Logs a SMD Lite log message by log level
 *
 * If the current logging level allows the specified logging level to be
 * logged, write it to the smem_log and target-specific log; otherwise,
 * suppress the message. An optimizing compiler ought to be able to drop the
 * log message at compile time.
 */
#if !defined(SMD_Q6FW_PROC) && !defined(SMD_DSPS_PROC) && !defined(SMD_RPM_PROC) && !defined(FEATURE_SMD_STATIC_LIB) && !defined (FEATURE_UEFI)
#define SMDL_LOG(level, fmt, a, b, c) \
	smdl_loglevel = SMDL_LOGLEVEL; \
	if (smdl_loglevel <= level) { \
    SMEM_LOG_EVENT(SMEM_LOG_SMDLITE_EVENT_BASE | __LINE__, \
        (uint32)a, (uint32)b, (uint32)c); \
  }
#else
#define SMDL_LOG(level, fmt, a, b, c)
#endif

/** Logs a debug message */
#define SMDL_DEBUG(fmt, a, b, c) SMDL_LOG(SMDL_LOGLEVEL_DEBUG, fmt, a, b, c)

/** Logs an info message */
#define SMDL_INFO( fmt, a, b, c) SMDL_LOG(SMDL_LOGLEVEL_INFO , fmt, a, b, c)

/** Logs a warning message */
#define SMDL_WARN( fmt, a, b, c) SMDL_LOG(SMDL_LOGLEVEL_WARN , fmt, a, b, c)

/** Logs an error message */
#define SMDL_ERROR(fmt, a, b, c) SMDL_LOG(SMDL_LOGLEVEL_DEBUG, fmt, a, b, c)

#endif /* SMDL_DEBUG_H */
