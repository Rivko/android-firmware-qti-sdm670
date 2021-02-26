#ifndef SECERRNO_H
#define SECERRNO_H

/** @file secerrno.h
 * @brief
 * This contains the definition of the Security Services return
 * codes (error numbers). Functions using this definition return
 * an error code to indicate the status of execution.
 * 
 */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secerrno.h_v   1.9   30 Aug 2003 20:19:56   omichael  $


when       who     what, where, why
--------   ---     -----------------------------------------------------------------------
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
09/09/09   bm      removed FEATURE_SEC for CMI Compliance
06/10/09   ejt     removed PLATFORM_LTK around #include "IxErrno.h"
06/01/05   rv      undefined 'errno' & '_errno' defined by VS .NET for sirius LTK builds.
06/10/02   om      Changed secerrno_enum_type to be typedef'd.
05/07/01   yll     Added certificate parsing and verification related errno.
10/23/00   om      Created Module

Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==========================================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "IxErrno.h"

/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/

/** Map the SEC errno type to the common errno */
typedef errno_enum_type secerrno_enum_type;

/** Callback fptr type */
typedef void (*secerrno_cb_fptr_type)( void*, secerrno_enum_type );

#endif /* SECERRNO_H */
