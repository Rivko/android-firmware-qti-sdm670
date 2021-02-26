#ifndef DSPLIBS_TRIGNM_H
#define DSPLIBS_TRIGNM_H
/*============================================================================
  @file dsplibs_trignm.h

  Declare methods for trigonometric operations.

Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "comdef.h"
#include "basic_op_old.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*! 
Approximates sin(2*PI*x)

\param x input in Q24

\details

\return
sin(2*PI*x) in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 6 (x<0)
 - 5 (x >=0)

\b Notes
 - Use 512-entry LUT with interpolation for approximation

*/

Word32 dsplib_sin(Word32 x);


/*! 
Approximates cos(2*PI*x)

\param x input in Q24

\details

\return
cos(2*PI*x) in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 6 (x<0)
 - 5 (x >=0)

\b Notes
 - Use 512-entry LUT with interpolation for approximation

*/

Word32 dsplib_cos(Word32 x);



/*! 
Approximates arctan(x)/PI for x in [-1,1)

\param x input in Q15

\details

\return
arctan(x)/PI in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 4

\b Notes
 - Use polynomial approximation with MSE

*/

Word32 dsplib_atan(Word16 x);

#endif /* #ifndef DSPLIBS_TRIGNM */
