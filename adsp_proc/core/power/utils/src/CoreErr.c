/*==============================================================================
@file CoreErr.c

Unrecoverable error handler definitions

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreErr.c#1 $
==============================================================================*/
#include "CoreErr.h"
#include "CoreVerify.h"

/**
   @name _DefaultErrFatalHandler
   @brief default handler function for critical errors
*/
static void _DefaultErrFatalHandler( const char *fmt,
                                     const char *filename,
                                     int line,
                                     va_list argp )
{
  ERR_FATAL("FATAL ERROR in file %s at line %d",filename, line, 0);
}

static CoreErrHandler_t _ErrFatalHandler = _DefaultErrFatalHandler;

/**
   @name CoreErr_RegisterErrFatalHandler
   @brief Register a function to be called when the process encounters a critical error
*/
void CoreErr_RegisterErrFatalHandler( CoreErrHandler_t pf )
{
  if ( NULL != pf )
  {
    _ErrFatalHandler = pf;
  }
}

void CoreErr_OnErrFatal( const char *fmt,
			 const char *filename,
			 int line,
			 ... )
{
  va_list argp;
  va_start( argp, line );
  _ErrFatalHandler( fmt, filename, line, argp );
  /* will never get here... */
  va_end( argp );
}
