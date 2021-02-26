#ifdef AEEBEGINPACK_H
#error "Improper inclusion of AEEbeginpack.h - see AEEbeginpack.h for usage"
#endif /* #ifdef AEEBEGINPACK_H */

#define AEEBEGINPACK_H
/*
==========================================================================

File:         AEEbeginpack.h

Description:  Structure packing pragma's and declarations

Usage:
              typedef struct foo foo;
              #include "AEEbeginpack.h"
              struct foo {
                 <structure parameters>
              }
              #include "AEEendpack.h"
              ;

              Notes
              1. Semi-colon follows the inclusion of AEEendpack.h
              2. Each structure to be packed is wrapped with these header
                 inclusions
============================================================================
*/
/*==============================================================================
  Copyright (c) 206-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */
#if defined(_MSC_VER)

#pragma warning(disable:4103)  /* another header changing "pack" */
#pragma pack(push,1)

#elif defined(__ARMCC_VERSION)

  __packed

#elif defined(__GNUC__)

  // do nothing

#else
#error "Unsupported compiler"
#endif

/*
==========================================================================
  DATA STRUCTURES DOCUMENTATION
==========================================================================

==========================================================================
CONSTANTS DOCUMENTATION
==========================================================================

==========================================================================
*/
