#ifndef __PARSER_LIB_H__
#define __PARSER_LIB_H__

/*=============================================================================

                     Parser  Lib Information  Header File

GENERAL DESCRIPTION
  This file contains definitions and functions for Smbios config
  file parser

Copyright 2012-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/15   va      Secton support for csv by section file
09/23/14   yg      Add API for csv by section and reopen
11/01/13   vk      Fix 64 bit compiler warnings
07/09/12   yg      Make parser Generic
03/23/12   kpa     Initial creation
            
============================================================================*/
/*
*  Config file format brief explanation:
*  All comments start with ; ending with newline
*  Space of anytype allowed everywhere except on the config line, unless the
*  space is needed in the config item itself. Only spaces enclosed in double
*  quotes are preserved in parsed token strings. Its user responsibility to
*  ensure matching start and end double quotes.
*  Individual Tokens are seperated by ',' or ':'
*
*/

#include "Uefi.h"

/*
*  Callback function type for separated Tokens. token_ptr_array contains pointers
*  to individual token strings. The Callback Function must interprete the parsed
*  tokens 
*
* */
typedef void(*TokenCallBackFuncType)(UINT8* TokenPtrArray[], UINTN MaxTokens);

/*
*  Callback function type for separated Tokens and section string.
*  Token_ptr_array contains pointers to individual token strings. The Callback
*  Function must interprete the parsed tokens 
*  The SectionOffset returns the running count of the sections to handle duplicate
*  sections
* */
typedef void(*TokenSectionCallBackFuncType)(UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens);

/* Callback function type to get enumerated Key Value Pairs
 * If Success 0 is returned then the enumeration will continue
 * If failure -1 is returned then enumeration will stop */
typedef void(*KeyValueCallBackFuncType)(UINT8* Section, UINT8* Key, UINT8* Value);


#define SUPPRESS_BYTEORDERMARK           (1 << 0)
#define IGNORE_SPACE_IN_VALUE            (1 << 1)
#define ENABLE_COMMA_DELIMITER           (1 << 2)
#define ENABLE_SEMICOLON_DELIMITER       (1 << 3)
#define DISABLE_COLON_DELIMITER          (1 << 4)

typedef struct {
  UINT8     CommentChar;
  UINT8     Res1;
  UINT16    Res2;
  UINT32    Options;
}ParseOptionsType;

/* Open Parser for the buffer passed in
 * If ParseOptions pointer is NULL then default values are used (ie # is comment char)
 *
 * Buffer     : Buffer containing the data to be parsed
 * BufferSize : Size of the data in buffer
 * Popt       : ParseOptions (Only comment char is supported now)
 *
 * Return the Parser descriptor to pass to next functions
 * */
INTN
OpenParser (IN  UINT8*             Buffer,
            IN  UINT32             BufferSize,
            IN  ParseOptionsType*  Popt OPTIONAL);

/*
 *  Enumerate through the comma delimited config items via callback.
 *
 *  Pd           : Parser Descriptor associated with the buffer of interest
 *  CallBackFunc : Call back function which will be called with all
 *                 the config items found in the Parsed buffer
 *
 *  Returns 0 on Success and -ve value on failure
 *
 */
INTN
EnumCommaDelimItems (IN  INTN                   Pd,
                     IN  TokenCallBackFuncType  CallBackFunc);

/*
 *  Enumerate through the comma delimited config items for a given section via callback.
 *
 *  Pd           : Parser Descriptor associated with the buffer of interest
 *  Section      : Pointer to section string of interest
 *  CallBackFunc : Call back function which will be called with all
 *                 the config items found in the Parsed buffer
 *
 *  Returns 0 on Success and -ve value on failure
 *
 */
INTN
EnumCommaDelimSectionItems (IN  INTN                   Pd,
                            IN  UINT8*                 Section   OPTIONAL,
                            IN  TokenSectionCallBackFuncType  CallBackFunc);

/*
 * Iterate through the key values individually. Steps through every item and returns the
 * section it belongs to as well. The buffer is held by Parser and source should NOT be
 * modified
 *
 * Pd       : Parser Descriptor
 * Sectionp : Pointer to UINT8* where pointer to section string can be returned.
 * Keyp     : Pointer to UINT8* where pointer to key string can be returned.
 * Valp     : Pointer to UINT8* where pointer to value string can be returned.
 *
 * Returns 0 on success and -ve value on failure
 * */
INTN
GetNextKeyValue (IN  INTN     Pd,
                 IN  UINT8**  Sectionp,
                 IN  UINT8**  Keyp,
                 IN  UINT8**  Valp);

/*
 * Enumerate the key values by getting callback for each item. Optional Section string can
 * be specified to get the values only for the specified section. If the Section pointer is
 * NULL then all values with their Section string is returned.
 *
 * Pd            : Parser Descriptor
 * Section       : Optional Section, if specified, only key-value pairs for the specified section are returned.
 * CallBackFunc  : Callback function to get the callback with all the enumerated values.
 *
 * Returns 0 on success and -ve value on failure
 * */
INTN
EnumKeyValues (IN  INTN                      Pd,
               IN  UINT8*                    Section   OPTIONAL,
               IN  KeyValueCallBackFuncType  CallBackFunc);

/*
 * Re-open parser for repeated further processing
 *
 * Pd       : Parser Descriptor
 *
 * Returns 0 on success and -ve value on failure
 * */
INTN
ReopenParser (IN INTN Pd);

/*
 * Close the parser context and release the resources.
 *
 * Pd       : Parser Descriptor
 *
 * Returns 0 on success and -ve value on failure
 * */
INTN
CloseParser (IN  INTN            Pd);

#endif /* __PARSER_LIB_H__ */


