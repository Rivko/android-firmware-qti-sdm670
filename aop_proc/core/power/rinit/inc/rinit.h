#ifndef __RINIT_H__
#define __RINIT_H__
/*===========================================================================
                                 rinit.h

DESCRIPTION:
  This file defines type defs and APIs to call rinit module for initializing
  different Hardware blocks.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "comdef.h"

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
/* Enum to indicate return code from rinit APIs */
enum rinit_err_t
{
  RINIT_SUCCESS = 0,
  RINIT_UNKNOWN_MODULE = 1,          /* Unknown/unhandled module for Rinit */
  RINIT_EXT_ERR = 2,                 /* Extension returned an error */
  RINIT_INTERNAL_ERR = 0x7FFFFFFF    /* Forces it 32 bit value */
};

/* Structure containing information about HW block and initialization values */
typedef struct rinit_module_entry
{
  const char *name;             /* name of hw block/module */
  size_t      count;            /* size of values below */
  uint32     *values;           /* init values for the hw block */
} rinit_module_entry;

//===========================================================================
//                       Function Declaration
//===========================================================================
/**
 * <!-- rinit_module_init -->
 *
 * @brief Initializes the HW module indicated by input name
 *
 * @param name: Name of the module that needs to be initialized
 *
 * @return 0 if module initialization was successful else rinit_err_t
 */
int rinit_module_init (const char *name);

#endif  // __RINIT_H__
