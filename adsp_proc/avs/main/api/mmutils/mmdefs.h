#ifndef _MMDEFS_H
#define _MMDEFS_H
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

  Defines common types used within Multimedia Subsystem. Attempt is made to
  align to C99 standard and intention is to keep the list of commonly used
  types small.

  Copyright (C) 2010 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/avs.adsp/2.8.5/api/mmutils/mmdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2010/05/17 act     Removing SWIG. Updating to use stdint.h on compilers that
                   we support.
2010/05/12 act     Moving TRUE/FALSE, bool, and char types outside STDINT_H
                   protection since C99 doesn't define those types.
2010/05/11 act     Adding char_t type.
2010/04/29 act     Added protection for redefinition of C99 types. Changed
                   _WIN32 check.
2010/04/27 act     Changed a few comments, removed some less-used definitions
2010/04/23 PR      Initial revision from comdef.h (stripped version)
===========================================================================*/

/* -----------------------------------------------------------------------
** Standard Integer Types
** ----------------------------------------------------------------------- */

#if /* Supported Compilers */ \
    defined(__ARMCC_VERSION) || \
    defined(__GNUC__)

  /* If we're hosted, fall back to the system's stdint.h, which might have
   * dditional definitions.
   */

  #include "stdint.h"

#else /* Unsupported Compilers */

  /* The following definitions are the same accross platforms.  This first
   * group are the sanctioned types.
   */

  typedef unsigned long long uint64_t;  /* Unsigned 64 bit value */
  typedef unsigned long int  uint32_t;  /* Unsigned 32 bit value */
  typedef unsigned short     uint16_t;  /* Unsigned 16 bit value */
  typedef unsigned char      uint8_t;   /* Unsigned 8  bit value */

  typedef signed long long   int64_t;   /* Signed 64 bit value */
  typedef signed long int    int32_t;   /* Signed 32 bit value */
  typedef signed short       int16_t;   /* Signed 16 bit value */
  typedef signed char        int8_t;    /* Signed 8  bit value */

#endif /* Supported Compilers */

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#undef TRUE
#undef FALSE

#define TRUE   (1)  /* Boolean true value. */
#define FALSE  (0)  /* Boolean false value. */

#ifndef NULL
  #define NULL (0)
#endif

/* ------------------------------------------------------------------------
** Character and boolean
** ------------------------------------------------------------------------ */
typedef char char_t;           /* Character type */
typedef unsigned char bool_t;  /* Boolean value type. */

#endif /* _MMDEFS_H */

