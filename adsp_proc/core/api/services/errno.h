#ifndef ERRNO_H
/*lint -e749 -e750 -e751*/
#define ERRNO_H

/**
  @file errno.h
  @brief This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_error_codes" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

               E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.
 

Copyright (c) 2000-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MM_DATA/vcs/errno.h_v   1.0   08 Aug 2002 11:19:54   akhare  $
$Header: //components/rel/core.qdsp6/2.1/api/services/errno.h#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
03/10/06    sb     Need to include stdlib.h for LTK (.Net) builds
03/07/06    rv     Temporary fix for M6280XXX3003 builds
15/01/06    gr     Replaced body of errno.h with inclusion of the IxErrno.h
                   file. This is to help avoid conflicts with system errno
                   files. 
04/30/03    gr     Added inclusion of errno_codes.h and definition of errno
                   macro.
04/23/02   rwh     Added the E_BAD_DATA value and grouping from secerrno.h
09/24/01   ajn     Renamed the file from hdrerrno.h to errno.h, since it
                   contains no HDR specific content and is useful for IS-95
                   A & B, and IS2000 1x.
05/05/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup utils_error_codes
@{ */

#ifdef PLATFORM_LTK
#include <stdlib.h>
#endif

//#include "IxErrno.h"

/** @} */ /* end_addtogroup utils_error_codes */
#endif /* ERRNO_H */
