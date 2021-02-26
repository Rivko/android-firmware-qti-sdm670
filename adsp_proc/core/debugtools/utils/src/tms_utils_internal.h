#ifndef TMS_UTILS_INTERNAL_H
#define TMS_UTILS_INTERNAL_H
/**
@file tms_utils_internal.h
@brief This file contains the Macro definitions for TMS utils
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2016 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/utils/src/tms_utils_internal.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#define ULONG_BITS      32          // maximum buffer required to hold numeric template output

#define FIELD_FLAG_NONE   0
#define FIELD_FLAG_ZERO   1         // flag, zero pad
#define FIELD_FLAG_LNEG   2         // flag, left negative
#define FIELD_FLAG_LONG   4         // flag, format template indicates long value
#define FIELD_FLAG_SIGN   8         // flag, include sign

#define NUMERIC_ZERO    0           // NUMERIC ZERO

#define ASCII_NIL       '\0'        // ASCII NULL
#define ASCII_ZERO      '0'         // ASCII ZERO
#define ASCII_9         '9'         // ASCII
#define ASCII_SPACE     ' '         // ASCII SPACE
#define ASCII_SIGN      '-'         // ASCII
#define ASCII_PERCENT   '%'         // ASCII

#define ASCII_CAP_B     'B'         // ASCII
#define ASCII_CAP_C     'C'         // ASCII
#define ASCII_CAP_D     'D'         // ASCII
#define ASCII_CAP_L     'L'         // ASCII
#define ASCII_CAP_O     'O'         // ASCII
#define ASCII_CAP_S     'S'         // ASCII
#define ASCII_CAP_U     'U'         // ASCII
#define ASCII_CAP_X     'X'         // ASCII

#define ASCII_A         'a'         // ASCII
#define ASCII_D         'd'         // ASCII
#define ASCII_L         'l'         // ASCII
#define ASCII_X         'x'         // ASCII


#endif /* TMS_UTILS_INTERNAL_H */
