/*===========================================================================

FILE:         qdss_parse_util.c

DESCRIPTION:  Utilities to aid parsing a null-terminated string.

===========================================================================*/
/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdlib.h>              // strtoul
#include "stringl/stringl.h"     //strlen, memset
#include "com_dtypes.h"
#include "qdss_parse_util.h"

/*---------------------------------------------------------------------------
 * Static Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Check for decimal format of an unsigned long.
   Returns
      QDSS_PARSE_UTIL_RVAL_SUCCESS - String is a valid format
      QDSS_PARSE_UTIL_RVAL_INVALID_ARG - String is not a valid format
         format. Valid format must not start with "0x".
      QDSS_PARSE_UTIL_RVAL_FAILED - Invalid Null pointer specified.
      QDSS_PARSE_UTIL_RVAL_MISSING_ARG - String to convert is empty.
*/
int qpu_is_valid_hexstr (char *val_str)
{
   unsigned int ii;
   size_t val_str_len;
   if (NULL == val_str)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   val_str_len = strlen(val_str);
   if (0 == val_str_len)
   {  // Nothing to convert.
      return  QDSS_PARSE_UTIL_RVAL_MISSING_ARG;
   }
   if (2 > val_str_len)  // Prefix "0x" missing
   {
      return  QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   if ('0' != val_str[0])
   {  // First char not '0'
      return  QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   if ('x' != val_str[1] && 'X' != val_str[1])
   {  // Second char not 'x'
      return  QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }

   // Partial range check.
   // Unsigned long max value is 0xFFFF FFFF
   ii = 2;
   while ((8 < val_str_len - ii) && ('0' == val_str[ii]))
   {  // Advance past the leading zeros.
      ii++;
   }
   if (8 < val_str_len - ii)
   {  // Too many parameter
      return  QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   for (ii = 2; ii < val_str_len; ii++)
   {  // Check for valid characters.
      switch (val_str[ii])
      {
         case '0':
         case '1': case '2': case '3':
         case '4': case '5': case '6':
         case '7': case '8': case '9':
         case 'a': case 'A':
         case 'b': case 'B':
         case 'c': case 'C':
         case 'd': case 'D':
         case 'e': case 'E':
         case 'f': case 'F':
            break;
         default:
            return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
            //break;
      }
   }
   return  QDSS_PARSE_UTIL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
/* Check for octal format of an unsigned long.
   Returns
      QDSS_PARSE_UTIL_RVAL_SUCCESS - String is a valid format
      QDSS_PARSE_UTIL_RVAL_INVALID_ARG - String is not a valid format
         format. Valid format must start with zero, '0'.
      QDSS_PARSE_UTIL_RVAL_FAILED - Invalid Null pointer specified.
      QDSS_PARSE_UTIL_RVAL_MISSING_ARG - String to convert is empty.
*/
int qpu_is_valid_octalstr(char* val_str)
{
   unsigned int ii;
   size_t val_str_len;
   if (NULL == val_str)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   val_str_len = strlen(val_str);
   if (0 == val_str_len)
   {  // Nothing to convert.
      return  QDSS_PARSE_UTIL_RVAL_MISSING_ARG;
   }
   if ('0' != val_str[0])
   {  // Must start with zero, '0'.
      return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   // Partial range check.
   // Unsigned long max value is 037 777 777 777
   ii = 1;
   while ((11 < val_str_len - ii) && ('0' == val_str[ii]))
   {  // Advance past the leading zeros.
      ii++;
   }
   if ( (11 < val_str_len - ii) ||
        ((11 == (val_str_len-ii)) && ('3' < val_str[ii]))
      )
   {
      return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   else
   {
      for (ii = 1; ii < val_str_len; ii++)
      {  // Check for valid characters.
         switch (val_str[ii])
         {
            case '0': case '1':
            case '2': case '3':
            case '4': case '5':
            case '6': case '7':
               break;
            default:
               return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
         }
      }
      return QDSS_PARSE_UTIL_RVAL_SUCCESS;
   }
}

/*-------------------------------------------------------------------------*/
/* Check for decimal format of an unsigned long.
   Returns
      QDSS_PARSE_UTIL_RVAL_SUCCESS - String is a valid format
      QDSS_PARSE_UTIL_RVAL_INVALID_ARG - String is not a valid format
         format. Valid format must not start with zero, '0' unless value
         is zero.
      QDSS_PARSE_UTIL_RVAL_FAILED - Invalid Null pointer specified.
      QDSS_PARSE_UTIL_RVAL_MISSING_ARG - String to convert is empty.
*/
int qpu_is_valid_decimalstr(char* val_str)
{
   unsigned int ii;
   size_t val_str_len;
   if (NULL == val_str)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   val_str_len = strlen(val_str);
   if (0 == val_str_len)
   {  // Nothing to convert.
      return QDSS_PARSE_UTIL_RVAL_MISSING_ARG;
   }
   if ('0' == val_str[0])
   {
      if (1 == val_str_len)
      {  // Accept as a zero decimal value (as oppose to octal)
         return QDSS_PARSE_UTIL_RVAL_SUCCESS;
      }
      else
      {  // Octal format
         return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
      }
   }
   // Partial range check.
   // Unsigned long max value is 4,294,967,295
   if (10 < val_str_len)
   {
      return QDSS_PARSE_UTIL_RVAL_INVALID_ARG; // Value is too large.
   }
   else if (10 == val_str_len)
   {
      if ('4' < val_str[0])
      {  return QDSS_PARSE_UTIL_RVAL_INVALID_ARG; } // Value too large.
      else if ('4' == val_str[0] && '2' < val_str[1])
      {  return QDSS_PARSE_UTIL_RVAL_INVALID_ARG; } // Value too large.
      else
      {  return QDSS_PARSE_UTIL_RVAL_SUCCESS; }
   }
   else
   {  // Acceptable numeric characters.
      for (ii = 0; ii < val_str_len; ii++)
      {  // Check for valid characters.
         switch (val_str[ii])
         {
            case '0':
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9':
               break;
            default:
               return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
         }
      }
      return QDSS_PARSE_UTIL_RVAL_SUCCESS;
   }
}

/*-------------------------------------------------------------------------*/
/* Convert specified "val_str" string to unsigned long "*val".
   "val_str" can be hex 0x, octal 0, or decimal numeric format.
   Return:
      QDSS_PARSE_UTIL_RVAL_SUCCESS     Successful
      QDSS_PARSE_UTIL_RVAL_FAILED      Pointer argument invalid
      QDSS_PARSE_UTIL_RVAL_MISSING_ARG Value string is empty
      QDSS_PARSE_UTIL_RVAL_INVALID_ARG Value format is invalid.
*/
int qpu_convert_to_ul(unsigned long *val, char *val_str)
{
   if (NULL == val)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   *val = 0;
   if (NULL == val_str)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   if (0 == strlen(val_str))
   {  // String is empty.
      return QDSS_PARSE_UTIL_RVAL_MISSING_ARG;
   }

   // Check format for hex first, octal, and lastly decimal.
   if (QDSS_PARSE_UTIL_RVAL_SUCCESS ==
       qpu_is_valid_hexstr(val_str))
   {
      *val = (unsigned long)strtoul(val_str, (char**)NULL, 16);
   }
   else if (QDSS_PARSE_UTIL_RVAL_SUCCESS ==
            qpu_is_valid_octalstr(val_str))
   {
      *val = (unsigned long)strtoul(val_str, (char**)NULL, 8);
   }
   else if (QDSS_PARSE_UTIL_RVAL_SUCCESS ==
            qpu_is_valid_decimalstr(val_str))
   {
      *val = (unsigned long)strtoul(val_str, (char**)NULL, 10);
   }
   else
   {
      return QDSS_PARSE_UTIL_RVAL_INVALID_ARG;
   }
   return QDSS_PARSE_UTIL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
/* Returns true if character c matches a character in the null-terminated
   string delims. Otherwise, returns FALSE.
*/
boolean qpu_is_delimiter (char c, const char *delims)
{
   int ii = 0;

   if (NULL == delims)
   {
      return FALSE;
   }
   while ('\0' != delims[ii])
   {
      if (c == delims[ii])
      {
         return TRUE;
      }
      ii++;
   }
   return FALSE;
}

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
int qdss_parse_util_get_token(char *ret_token,
                              size_t ret_token_size,
                              const char **parse_line_ptr,
                              char *delims)
{
   unsigned int token_idx = 0;
   char current_char;

   if (NULL == ret_token)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }
   memset(ret_token, '\0', ret_token_size); // Self terminate entire string.

   if (NULL == parse_line_ptr || NULL == *parse_line_ptr || NULL == delims)
   {
      return QDSS_PARSE_UTIL_RVAL_FAILED;
   }

   // Find start of token (remove any delimiters).
   current_char = **parse_line_ptr;
   while (qpu_is_delimiter(current_char, delims) &&
         current_char != '\0')
   {
      (*parse_line_ptr)++;
      current_char = **parse_line_ptr;
   }

   // Collect token characters.
   while (!qpu_is_delimiter(current_char, delims) &&
          current_char != '\0')
   {
      if (token_idx >= (ret_token_size - 1))
      {  // Max bytes exceeded. Account for terminating NULL.
         return QDSS_PARSE_UTIL_RVAL_FAILED;
      }
      ret_token[token_idx] = **parse_line_ptr;
      token_idx++;
      (*parse_line_ptr)++;
      current_char = **parse_line_ptr;
   }
   return (0 == strlen(ret_token)) ?
      QDSS_PARSE_UTIL_RVAL_FAILED :
      QDSS_PARSE_UTIL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
int qdss_parse_util_get_ul(unsigned long *ret_val,
                           const char **parse_line_ptr,
                           char *delims)
{
   char val_str[16];  // ascii string of unsigned long number
   int rval;

   *ret_val = 0;

   // Get the next token.
   rval = qdss_parse_util_get_token(
      val_str, sizeof(val_str), parse_line_ptr, delims);

   if (QDSS_PARSE_UTIL_RVAL_SUCCESS == rval)
   {
      // Convert to a unsigned long value.
      rval = qpu_convert_to_ul(ret_val, val_str);
   }
   else
   {
      rval = QDSS_PARSE_UTIL_RVAL_MISSING_ARG;
   }

   return rval;
}
