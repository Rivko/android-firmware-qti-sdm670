#ifndef QDSS_PARSE_UTIL_H
#define QDSS_PARSE_UTIL_H
/**
  File: qdss_parse_uitl.h

  QUALCOMM Debug Subsystem (QDSS) utilities for parsing a Null terminated
  string.
*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/ctrl/src/qdss_parse_util.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  12/11/14   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <stddef.h>  //size_t

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define QDSS_PARSE_UTIL_RVAL_SUCCESS      0
#define QDSS_PARSE_UTIL_RVAL_FAILED       1
#define QDSS_PARSE_UTIL_RVAL_MISSING_ARG  2
#define QDSS_PARSE_UTIL_RVAL_INVALID_ARG  3

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/** Function: qdss_parse_util_get_token

   Dependencies
      None.

   Parameters
      ret_token      [out] - Pointer to the buffer to copy the null
                             terminated string of token.
                             Valid when QDSS_PARSE_UTIL_RVAL_SUCCESS.
      ret_token_size [out] - Number of bytes available as pointed to by
                             ret_token.
      parse_line_ptr [in ] - Pointer to pointer to the null terminated
                             string to parse.
                     [out] - Pointer updated to one character after the
                             last character of the token found.
      delims         [in ] - Pointer to the null terminated string of
                             delimiter characters.

   Return
      QDSS_PARSE_UTIL_RVAL_SUCCESS - Token found.
      QDSS_PARSE_UTIL_RVAL_FAILED - No token found or SW error (
         invalid NULL pointer argument specified, exceeded memory).
*/
int qdss_parse_util_get_token(char *ret_token,
                              size_t ret_token_size,
                              const char **parse_line_ptr,
                              char *delims);

/*-------------------------------------------------------------------------*/
/** Function: qdss_parse_util_get_ul

   Parses the null terminated string for the next token and converts
   token string to a unsigned long value.

   Dependencies:
      None.

   Parameters:
      val            [out] - Pointer to place resulting unsigned long
                             integer value represented by string val_str.
                             Valid when QDSS_PARSE_UTIL_RVAL_SUCCESS.
      parse_line_ptr [in ] - Pointer to pointer to the null terminated
                             string to parse.
                     [out] - Pointer to string is updated to one character
                             after the last character of the token found.
      delims         [in ] - Pointer to the null terminated string of
                             delimiter characters.

   Return:
      QDSS_PARSE_UTIL_RVAL_SUCCESS - Successful parsed for a unsigned
                                     long (see val).
      QDSS_PARSE_UTIL_RVAL_FAILED - SW error (invalid NULL pointer arg)
      QDSS_PARSE_UTIL_RVAL_MISSING_ARG - Value to be convert was not found.
      QDSS_PARSE_UTIL_RVAL_INVALID_ARG - Value is invalid, format error.
*/
int qdss_parse_util_get_ul(unsigned long *ret_val,
                           const char **parse_line_str,
                           char *delims);

#endif /* #ifndef QDSS_PARSE_UTIL_H */

