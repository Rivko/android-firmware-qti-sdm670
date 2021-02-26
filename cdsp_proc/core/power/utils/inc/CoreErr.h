/*==============================================================================
@file CoreErr.h

Unrecoverable error handler declarations

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreErr.h#1 $
==============================================================================*/
#ifndef COREERR_H
#define COREERR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype for registered error fatal function */
typedef void (*CoreErrHandler_t)( const char *fmt,
                                  const char *filename,
                                  int line,
                                  va_list argp );

/**
  @name  - CoreErr_OnErrFatal
  @brief - Significant failure in software that will no longer allow normal processing
  for that element.

  The default error handler will execute the following:

  * An indication will be sent to the process for error failure recovery.
  * Line number and file failure point will be logged.
  * If a debugger is attached (JTAG or IDE), the debugger will automatically break.
  * If the debugger is NOT attached, SIGBUS will automatically be raised.
  * In the context of QNP Platform Processes, this SIGBUS will be handled through signaltrap

  Otherwise, if the program registers an alternate critical error
  handling function by calling CoreErr_RegisterErrFatalHandler, then that
  registered function will be called.
**/
void CoreErr_OnErrFatal( const char *fmt,
                    const char *file,
                    int line,
                    ... );

/*
 * Function to register failure function.  
 * If no function is registered a default will be used
 */
void CoreErr_RegisterErrFatalHandler( CoreErrHandler_t );

#define CORE_ERR_FATAL(fmt, a, b, c) CORE_PROCESS_ERR_FATAL(fmt, a, b, c)

#define CORE_PROCESS_ERR_FATAL(fmt, ...) \
  CoreErr_OnErrFatal( fmt, __FILENAME__, __LINE__, ##__VA_ARGS__ )


#ifdef __cplusplus
}
#endif


#endif
