#ifndef ASSERT_H
#define ASSERT_H

/**
  @file assert.h
  @brief Definitions for ASSERT macro.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_assert" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/

/*==========================================================================

            A S S E R T   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  Definitions for ASSERT macro.
  
Copyright (c) 1998-2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/assert.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/12   dh      Check BUILD_BOOT_CHAIN flag for assert
01/24/12   dh      Modify assert to remove unreachable code warning if there is
                   any code is after an assert expression.
01/16/12   rks     created 
===========================================================================*/


/*===========================================================================
 
                            INCLUDE FILES

===========================================================================*/


/*===========================================================================
 
                            CONSTANT DEFINITIONS

===========================================================================*/


/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/


/**
  Given a boolean expression, ASSERT verifies that the condition is TRUE. If
  the condition is FALSE, ASSERT performs one of the following actions:
  
  - For ASSERT_FATAL -- Invokes ERR_FATAL processing.
  - For ASSERT_WARN -- Invokes ERR processing.
  - For all others -- No processing occurs.
@latexonly \newpage @endlatexonly
  Examples are:
  
  - ASSERT will trigger because i is equal to 5, and the assertion states 
    that i must NOT be 5:
<pre>
  int i=5;
  ASSERT( i != 5 );
</pre>

  - ASSERT will NOT trigger because i is not equal to 5, and the assertion states 
    that i must NOT be 5:
<pre>
  int i=4;
  ASSERT( i != 5 );
</pre>
  
  @param[in] xx_exp Condition to be asserted.
  
  @return
  None.

  @dependencies
   need to include boot_error_if.h
*/

#if (ASSERT==ASSERT_FATAL) && defined(BUILD_BOOT_CHAIN)

	#undef ASSERT

	#include "boot_error_if.h"

	#define ASSERT( exp ) \
  {\
    int err_Fatal = 1; \
    if(err_Fatal)\
    {\
      if( !(exp) ) \
      { \
        BL_ERR_FATAL(BL_ERR_OTHERS|BL_ERROR_GROUP_BOOT );\
      }\
    }\
   }
#elif !defined (QC_UEFI_ASSERT)

  #define ASSERT( xx_exp ) ((void)0)
  
#endif
	
#endif /* ASSERT.H */

