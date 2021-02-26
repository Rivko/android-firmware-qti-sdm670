/**
@file tms_utils_fmt.c
@brief This file contains the API for the TMS Utilities API 0.x
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/utils/src/tms_utils_num.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

// Code Size: ~1.5KB Optimized
// Stack Resource: ~80Byte + 4Byte * Vargs_Template_Expansion_Arguments

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "ctype.h"

#include "tms_utils.h"

#define NUMERIC_ZERO    0           // NUMERIC ZERO

#define ASCII_ZERO      '0'         // ASCII ZERO
#define ASCII_SIGN      '-'         // ASCII
#define ASCII_PLUS      '+'         // ASCII

#define ASCII_A         'a'         // ASCII

/**
API, Scan Input and Return Numeric Representation
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer (NULL Terminated)
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     long              Numeric Representation
*/
long tms_utils_num_l(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz)
{
   long rc = 0;
   TMS_UTILS_CHR_T sign;
   TMS_UTILS_CHR_T num_chr;
   TMS_UTILS_BUF_P num_str = in_buf_p;

   // Skip leading space

   while (num_str - in_buf_p < in_buf_sz && isspace(*num_str))
   {
      num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Record presense of a SIGN

   sign = num_chr = *num_str++;

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Skip over any SIGN presence

   if (ASCII_SIGN == num_chr || ASCII_PLUS == num_chr)
   {
      num_chr = *num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Loop Across Digits Obtain Value

   while (num_str - in_buf_p < in_buf_sz && isdigit(num_chr))
   {
      rc = 10 * rc + (num_chr - ASCII_ZERO);

      num_chr = *num_str++;
   }

   // If SIGN presence then adjust return

   if (ASCII_SIGN == sign)
   {
      return NUMERIC_ZERO - rc;
   }

   else
   {
      return rc;
   }
}

/**
API, Scan Input and Return Numeric Representation Hex
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     unsigned long     Numeric Representation
*/
unsigned long tms_utils_num_lx(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz)
{
   long rc = 0;
   TMS_UTILS_CHR_T num_chr;
   TMS_UTILS_BUF_P num_str = in_buf_p;

   // Skip leading space

   while (num_str - in_buf_p < in_buf_sz && isspace(*num_str))
   {
      num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // First Non Space; Does Not Check For Leading '0x'

   num_chr = *num_str++;

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Loop Across Digits Obtain Value

   while (num_str - in_buf_p < in_buf_sz && (isdigit(num_chr) || isalpha(num_chr)))
   {
      if (isdigit(num_chr))
      {
         rc = 16 * rc + (num_chr - ASCII_ZERO);
      }

      else if (isalpha(num_chr))
      {
         rc = 16 * rc + (tolower(num_chr) - ASCII_A + 10);
      }

      num_chr = *num_str++;
   }

   return rc;
}

/**
API, Scan Input and Return Numeric Representation
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer (NULL Terminated)
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     int               Numeric Representation
*/
int tms_utils_num_i(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz)
{
   return (int)tms_utils_num_l(in_buf_p, in_buf_sz);
}

/**
API, Scan Input and Return Numeric Representation Hex
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     unsigned int      Numeric Representation
*/
unsigned int tms_utils_num_ix(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz)
{
   return (unsigned int)tms_utils_num_lx(in_buf_p, in_buf_sz);
}

