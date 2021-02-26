#ifndef __JSON_PARSER__H_
#define __JSON_PARSER__H_
/*===========================================================================
                             json_parser.h

DESCRIPTION:
  This file implements the APIs to extract an attribute value from a simple
  JSON string.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*---------------------------------------------------------------------------
    Include Files
---------------------------------------------------------------------------*/
#include "stdio.h"

/*---------------------------------------------------------------------------
    Macro Definitions
---------------------------------------------------------------------------*/
#define JSON_PARSER_OK (0)

#define JSON_PARSER_INVALID				(-1)
#define JSON_PARSER_ATTR_NOT_FOUND		(-2)
#define JSON_PARSER_STRING_OVERFLOW		(-3)
#define JSON_PARSER_MALFORMED           (-4)
#define JSON_PARSER_INSUFFICIENT_BUFFER (-5)
#define JSON_PARSER_COPY_ERROR			(-6)
#define JSON_PARSER_INVALID_SIZE		(-7)

/*===========================================================================
                         Function Defintions
===========================================================================*/

/*
 * <!-- json_parser_err_to_string -->
 *
 * @brief Return the string associated with a specific API error code.
 *
 * @param err: Error code return by an API function.
 *
 * @return String associated with passed error code or
 *         "Unknown error" if not found.
 */
const char * json_parser_err_to_string( int err );

/*
 * <!-- json_parser_simple -->
 *
 * @brief This function takes a simple JSON string and extracts the data value
 *         associated with the attribute provided.
 *
 * @note Limitations:
 * @note   JSON data string cannot contain arrays
 * @note   JSON data string cannot contain nested attribute-value lists
 * @note   JSON attributes and valaues must not be quoted
 *
 * @param json: JSON data string which may or may not be NULL terminated.
 *              Example: "{class: rail, res : cx, att: afl, val : 128}"
 *
 * @param json_size: Number of characters in the JSON string counted from start
 *                   of pointer to ending curly bracket "}".
 *
 * @param attr: Desired attribute string which may or may not be NULL terminated.
 *              Example: "att".
 * @param attr_size: Size of attribute string excluding NULL.
 *
 * @param val: Pointer to buffer that will hold desired NULL terminated value.
 *
 * @param val_size: Size of val buffer including NULL.
 *
 * @return JSON_PARSER_*. See header file.
 *
 */
int json_parser_simple( char * json,
						size_t json_size,
						char * attr,
						size_t attr_size,
						char * val,
						size_t val_size );

#endif /* __JSON_PARSER__H_ */
