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
$Header: //components/rel/core.qdsp6/2.1/debugtools/utils/src/tms_utils_fmt.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

// Code Size: ~1.5KB Optimized
// Stack Resource: ~80Byte + 4Byte * Vargs_Template_Expansion_Arguments

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "tms_utils.h"
#include "tms_utils_internal.h"

/**
INTERNAL, Formatted Output to a Sized Buffer
Copy chr into out_p, respect length
*/
static inline TMS_UTILS_BUF_P tms_utils_put_chr(TMS_UTILS_BUF_P out_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_CHR_T chr)
{
   if (TMS_UTILS_NULL != out_p && TMS_UTILS_BUF_SZ_ZERO < out_buf_sz)
   {
      *out_p++ = chr;
   }

   return out_p;
}

/**
INTERNAL, Formatted Output to a Sized Buffer
Copy chr_p into out_p, respect length
*/
static inline TMS_UTILS_BUF_P tms_utils_put_chrs(TMS_UTILS_BUF_P out_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_BUF_P chr_p)
{
   int sz = out_buf_sz;

   if (TMS_UTILS_NULL != out_p && TMS_UTILS_BUF_SZ_ZERO < out_buf_sz && TMS_UTILS_NULL != chr_p)
   {
      while (TMS_UTILS_BUF_SZ_ZERO < sz && ASCII_NIL != *chr_p)
      {
         out_p = tms_utils_put_chr(out_p, sz--, *chr_p++);
      }
   }

   return out_p;
}

/**
INTERNAL, Formatted Output to a Sized Buffer
Copy argv into out_buf_p, respect length, use fmt_tpl for expansion, use argv as expansion arguments
*/
static TMS_UTILS_RET_T tms_utils_put(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_tpl, va_list argv)
{
   TMS_UTILS_BUF_P out_p = out_buf_p;

   // validate inputs

   if (TMS_UTILS_NULL != out_buf_p && TMS_UTILS_BUF_SZ_ZERO < out_buf_sz && TMS_UTILS_NULL != fmt_tpl)
   {
      // loop while room in out_p

      do
      {
         TMS_UTILS_CHR_T chr_num[ULONG_BITS + 1];
         TMS_UTILS_CHR_T fmt_chr, fmt_code;
         TMS_UTILS_ARG_T fmt_pad, fmt_fill;
         TMS_UTILS_ARG_T field_radix, field_width, field_flag, field_arg;

         fmt_chr = *fmt_tpl++;

         // end of fmt_tpl check

         if (ASCII_NIL == fmt_chr)
         {
            break;
         }

         // fmt_chr check for field replacement

         if (ASCII_PERCENT != fmt_chr)
         {
            out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), fmt_chr);

            continue;
         }

         // fmt_chr check for field alignment flags

         field_flag.ul = FIELD_FLAG_NONE;

         fmt_chr = *fmt_tpl++;

         // fmt_chr check for field alignment, FIELD_FLAG_ZERO padding

         if (ASCII_ZERO == fmt_chr)
         {
            field_flag.ul |= FIELD_FLAG_ZERO;

            fmt_chr = *fmt_tpl++;
         }

         // fmt_chr check for field alignment, left hand negative

         else if (ASCII_SIGN == fmt_chr)
         {
            field_flag.ul |= FIELD_FLAG_LNEG;

            fmt_chr = *fmt_tpl++;
         }

         // fmt_chr check for field alignment, field width

         for (field_width.ul = NUMERIC_ZERO; ASCII_ZERO <= fmt_chr && ASCII_9 >= fmt_chr; fmt_chr = *fmt_tpl++)
         {
            // multiply by 10, add next digit

            field_width.ul = field_width.ul * 10 + fmt_chr - ASCII_ZERO;
         }

         // fmt_chr check for field alignment, long value on stack (really a no-op)

         if (ASCII_L == fmt_chr || fmt_chr == ASCII_CAP_L)
         {
            field_flag.ul |= FIELD_FLAG_LONG;

            fmt_chr = *fmt_tpl++;
         }

         // end of fmt_tpl check, premature

         if (ASCII_NIL == fmt_chr) break;

         // normalize fmt_code from fmt_char field specifier

         fmt_code = fmt_chr;

         if (ASCII_A <= fmt_code)
         {
            fmt_code -= ASCII_SPACE;
         }

         switch (fmt_code)
         {
            default:                                                                   // next field is "whatever", copied directly
            {
               out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), fmt_chr);
               continue;
            }

            case ASCII_CAP_C:                                                          // next field is char, %c
            {
               out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), (TMS_UTILS_CHR_T)va_arg(argv, int));
               continue;
            }

            case ASCII_CAP_S:                                                          // next field is template, %s
            {
               TMS_UTILS_CHR_T *chrs_p = va_arg(argv, TMS_UTILS_BUF_P);

               // determine length of passed parameter template

               for (fmt_fill.ul = NUMERIC_ZERO; chrs_p && ASCII_NIL != *(chrs_p + fmt_fill.ul); fmt_fill.ul++)
                  /*NULL*/;

               // fill output buffer for justification

               while (FIELD_FLAG_NONE == (field_flag.ul & FIELD_FLAG_LNEG) && fmt_fill.ul++ < field_width.ul)
               {
                  out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), ASCII_SPACE);
               }

               // fill output buffer with passed parameter template

               out_p = tms_utils_put_chrs(out_p, out_buf_sz - (out_p - out_buf_p), chrs_p);

               // fill output buffer for remaining justification

               while (fmt_fill.ul++ < field_width.ul)
               {
                  out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), ASCII_SPACE);
               }

               continue;
            }

            case ASCII_CAP_B:                                                          // next field is numeric, pickup the radix, binary, %b
               field_radix.ul = 2;
               break;

            case ASCII_CAP_O:                                                          // next field is numeric, pickup the radix, octal, %o
               field_radix.ul = 8;
               break;

            case ASCII_CAP_D:
            case ASCII_CAP_U:                                                          // next field is numeric, pickup the radix, decimal, %d
               field_radix.ul = 10;
               break;

            case ASCII_CAP_X:                                                          // next field is numeric, pickup the radix, hex, %x
               field_radix.ul = 16;
               break;
         }

         // pickup field_arg.ul from stack, size properly, and determine sign

         if (FIELD_FLAG_NONE != (field_flag.ul & FIELD_FLAG_LONG))
         {
            field_arg.ul = va_arg(argv, long);
         }

         else
         {
            if (ASCII_D == fmt_code || ASCII_CAP_D == fmt_code)
            {
               field_arg.ul = (typeof(field_arg.l))va_arg(argv, int); // typecast (signed int)
            }
            else
            {
               field_arg.ul = (typeof(field_arg.l))va_arg(argv, unsigned int); // typecast (unsigned int)
            }
         }

         // absolute value of negative number as required for %d, %ld; this is performed with 2's compliment subtraction and typecasting

         if ((ASCII_D == fmt_code || ASCII_CAP_D == fmt_code) && NUMERIC_ZERO > (typeof(field_arg.l))field_arg.ul)
         {
            field_flag.ul |= FIELD_FLAG_SIGN;

            field_arg.ul = NUMERIC_ZERO - (typeof(field_arg.l))field_arg.ul;                                            // 2's compliment obtains absolute value
         }

         // copy field_arg.ul to chr_num with appropriate padding and sign

         fmt_pad.ul = NUMERIC_ZERO;

         do
         {
            // get the radix unit and radix remainder

            fmt_code = (TMS_UTILS_CHR_T)(field_arg.ul % field_radix.ul);
            field_arg.ul /= field_radix.ul;

            if (9 < fmt_code) // base adjustment for hex into ascii domain
            {
               if (ASCII_X == fmt_chr)
               {
                  fmt_code += 0x27; /* ascii lowercase offset */
               }
               else
               {
                  fmt_code += 0x07; /* ascii uppercase_offset */
               }
            }

            // copy adjusted chr value into chr_num

            chr_num[fmt_pad.ul++] = fmt_code + ASCII_ZERO;

         } while (NUMERIC_ZERO != field_arg.ul && sizeof(chr_num) > fmt_pad.ul);

         // sign handling

         if (FIELD_FLAG_NONE != (field_flag.ul & FIELD_FLAG_SIGN) && FIELD_FLAG_NONE == (field_flag.ul & FIELD_FLAG_ZERO))
         {
            if (sizeof(chr_num) > fmt_pad.ul)
            {
               chr_num[fmt_pad.ul] = ASCII_SIGN;
            }

            fmt_pad.ul++;
         }

         // zero or space pad handling

         if (FIELD_FLAG_NONE != (field_flag.ul & FIELD_FLAG_ZERO))
         {
            fmt_code = ASCII_ZERO;
         }
         else
         {
            fmt_code = ASCII_SPACE;
         }

         // output gets leading sign when needed

         if (FIELD_FLAG_NONE != (field_flag.ul & FIELD_FLAG_ZERO) && FIELD_FLAG_NONE != (field_flag.ul & FIELD_FLAG_SIGN))
         {
            out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), ASCII_SIGN);

            field_width.ul--;
         }

         // output gets left hand pad of space or zero

         fmt_fill.ul = fmt_pad.ul;

         while (FIELD_FLAG_NONE == (field_flag.ul & FIELD_FLAG_LNEG) && fmt_fill.ul++ < field_width.ul)
         {
            out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), fmt_code);
         }

         // output gets numeric value

         do
         {
            fmt_pad.ul--;

            if (sizeof(chr_num) > fmt_pad.ul)
            {
               out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), chr_num[fmt_pad.ul]);
            }

         } while (NUMERIC_ZERO != fmt_pad.ul);

         // output gets right hand pad of space

         while (fmt_fill.ul++ < field_width.ul)
         {
            out_p = tms_utils_put_chr(out_p, out_buf_sz - (out_p - out_buf_p), ASCII_SPACE);
         }

      } while (out_p < out_buf_p + out_buf_sz); // terminates when output is full

      // nil terminate the output area

      if (out_p < out_buf_p + out_buf_sz)
      {
         *out_p++ = ASCII_NIL;
      }

      else
      {
         *(out_buf_p + out_buf_sz - 1) = ASCII_NIL;
      }

      // return is the number of chars put to output area

      return out_p - out_buf_p;
   }

   // bad input buf_sz

   else
   {
      return TMS_UTILS_BUF_SZ_ZERO;
   }
}

/**
API, Formatted Output to a Sized Buffer
@param[in]  TMS_UTILS_BUF_P   Output Buffer Pointer (NULL Terminated on Output)
@param[in]  TMS_UTILS_BUF_SZ  Output Buffer Size
@param[in]  TMS_UTILS_FMT_P   Format Template Pointer (NULL Terminated)
@param[in]  ...               Format Template Expansion Arguments
@return     TMS_UTILS_RET_T   Byte Count Output into Buffer Pointer (>=0) w/ NULL Terminator, Error Indications (<0)
*/
TMS_UTILS_RET_T tms_utils_fmt(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_tpl, ...)
{
   TMS_UTILS_RET_T rc = TMS_UTILS_BUF_SZ_ZERO;

   if (TMS_UTILS_NULL != out_buf_p && TMS_UTILS_BUF_SZ_ZERO < out_buf_sz && TMS_UTILS_NULL != fmt_tpl)
   {
      va_list argv;
      va_start(argv, fmt_tpl);
      rc = tms_utils_put(out_buf_p, out_buf_sz, fmt_tpl, argv);
      va_end(argv);
   }

   return rc;
}
