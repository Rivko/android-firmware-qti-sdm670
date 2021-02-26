/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*!
 * \file
 * \brief       Internal SLIM utilities interface definition
 *
 * File contains types and macros that are part of SLIM platform internal API
 * and shall not be used by client's code.
 *
 * @ingroup slim_Log
 */

#ifndef _SLIM_LOG_UTILS_I_H_INCLUDED_
#define _SLIM_LOG_UTILS_I_H_INCLUDED_

#include <stdlib.h>

#include <slim_os_log_api.h>
#include <slim_os_api.h>

//! @addtogroup slim_Log
//! @{

//! Logging time type
/**
 * Logging subsystem uses UTC time with milliseconds component.
 */
typedef uint64_t slim_LogTimeT;

/**
@brief Function type for logging single line

@param[in] pzData  Logging context.
@param[in] tTimeMs Event time.
@param[in] pString Message to log.
*/
typedef void slim_LogPrintLineFnT
(
   const slim_LogDataT *pzData,
   slim_LogTimeT tTimeMs,
   const char *pString
);

//! @cond
slim_LogPrintLineFnT _slim_LogPrintLine;
slim_LogPrintLineFnT _slim_LogPrintOsLine;
slim_LogPrintLineFnT _slim_LogPrintConsole;
//! @endcond

//! Record system time with millisecond accuracy
/**
 * Function is used only by logging subsystem
 *
 * \return System time in milliseconds
 * \internal
 */
slim_LogTimeT _slim_LogGetTime();

/*!
 * @brief Get file name from logging request.
 *
 * @param[in] pz_Data Logging request data
 *
 * @return File name without path delimiters.
 */
const char *_slim_LogGetNakedFileName
(
  const slim_LogDataT *pzData
);

/*!
 * @brief Get logging flags for a given logging request.
 *
 * @param[in] pz_Data Logging request data
 *
 * @return Flags applicable to give request.
 */
uint32_t _slim_LogGetFlags
(
  const slim_LogDataT *pzData
);

//! @}

#endif /* _SLIM_LOG_UTILS_I_H_INCLUDED_ */
