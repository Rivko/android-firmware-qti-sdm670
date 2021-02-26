/*===========================================================================
                             json_parser.c

DESCRIPTION:
  This file implements the APIs to extract an attribute value from a simple
  JSON string.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================
                        Includes and Variable Definitions
===========================================================================*/

/*---------------------------------------------------------------------------
    Include Files
---------------------------------------------------------------------------*/
#include "stdio.h"

#include "json_parser.h"

/*---------------------------------------------------------------------------
    Internal Type Definitions
---------------------------------------------------------------------------*/
struct err_to_string_s {
	int err;
	char * string;
};

/*---------------------------------------------------------------------------
    Variables
---------------------------------------------------------------------------*/
static struct err_to_string_s _err_to_string[] = {
	{ JSON_PARSER_OK,                  "Success"				   },
	{ JSON_PARSER_INVALID, 		 	   "Invalid JSON string"	   },
	{ JSON_PARSER_ATTR_NOT_FOUND, 	   "Attribute not found"	   },
	{ JSON_PARSER_STRING_OVERFLOW, 	   "String overflow"		   },
	{ JSON_PARSER_MALFORMED, 		   "Malformed JSON string"	   },
	{ JSON_PARSER_INSUFFICIENT_BUFFER, "Insufficient buffer space" },
	{ JSON_PARSER_COPY_ERROR,		   "Could not copy string"	   },
	{ JSON_PARSER_INVALID_SIZE,		   "Length provided not valid" },
	{ 0,							   NULL	 					   }
};

/*===========================================================================
                         Function Defintions
===========================================================================*/

/*---------------------------------------------------------------------------
    Private Function Definition
---------------------------------------------------------------------------*/
/*
 * <!-- _verify_len -->
 *
 * @brief Verify the length of JSON string
 *
 * @param s: JSON String
 *
 * @param l: Length of JSON string
 *
 * @return JSON_PARSER_OK if successful else JSON_PARSER_INVALID_SIZE.
 */
static int _verify_len( const char * s, size_t l ){

	while ( l > 1 &&
		  	*s != '}'){
		l--;
		s++;
	}

	if ( l < 1 ||
	   	 *s != '}' ){
		return JSON_PARSER_INVALID_SIZE;
	}

	return JSON_PARSER_OK;
}

/*
 * <!-- _skip_spaces -->
 *
 * @brief Skip leading spaces in string
 *
 * @note The string passed is assumed to have a a non-space character present.
 *
 * @param s: String
 *
 * @return Pointer to first non-space character.
 */
static char * _skip_spaces( char * s ){
	while( *s == ' ' ){
		s++;
	}

	return s;
}

/*
 * <!-- _value_len -->
 *
 * @brief Find the length of the value.
 *
 * @note The string passed is assumed to have a terminating '}' present.
 *
 * @param s: Value string
 *
 * @return Length of value.
 */
static size_t _value_len( const char * s ){
	size_t l;

	l = 0;

	while( *s != '\0' &&
		   *s != ','  &&
		   *s != ' '  &&
		   *s != '}' ){
		s++;
		l++;
	}

	return l;
}

/*
 * <!-- _copy_val -->
 *
 * @brief Copy value from 'f' to 't'.
 *
 * @note The buffer is assumed to be large enought to hold the value string.
 *
 * @param f: Value string to copy from.
 *
 * @param t: Buffer to copy string to.
 *
 * @return Pointer to begining of 't'.
 */
static char * _copy_val( const char * f, char * t ){
	char * r;

	r = t;
	while( *f != '\0' &&
		   *f != ','  &&
		   *f != ' '  &&
		   *f != '}' ){
		*t = *f;
		t++;
		f++;
	}

	*t = '\0';

	return r;
}

/*
 * <!-- _compare_attr -->
 *
 * @brief Compare two attribute strings.
 *
 * @param j: JSON attribute.
 *
 * @param attr: Attribute desired.
 *
 * @param attr_size: Size of attribute desired.
 *
 * @return Pointer to begining of 'j' if successful else NULL.
 */
char * _compare_attr( char * j, char * attr, size_t attr_size ){

	while (( *j == *attr ) && attr_size ){
		j++;
		attr++;
		attr_size--;
	}

	if ( attr_size == 0 ){
		return j;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
    Public Function Definition
---------------------------------------------------------------------------*/
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
const char * json_parser_err_to_string( int err ){
	struct err_to_string_s * s;

	s = _err_to_string;

	while ( s->string != NULL ){
		if ( s->err == err ){
			return s->string;
		}
		s++;
	}

	return "Unknown error";
}

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
						size_t val_size ){
	char * j;
	char * p;
	size_t l;

	/* This call will ensure that a terminating '}' is precent in the string */
	if ( _verify_len( json, json_size ) != JSON_PARSER_OK ){
		return JSON_PARSER_INVALID_SIZE;
	}

	/* Ignore any leading spaces */
	j = _skip_spaces( json );

	/* The first character must be "{" */
	if ( *j != '{' ){
		return JSON_PARSER_MALFORMED;
	}

	/* Skip "{" */
	j++;

	/* Loop until the end of JSON is found */
	while( *j != '}' ){

		/* Ignore any leading spaces */
		j = _skip_spaces( j );

		/* Compare attribute in JSON string with one passed */
		p = _compare_attr( j, attr, attr_size );

		if ( p != NULL ){
			/* Ignore any leading spaces */
			j = _skip_spaces( p );

			/* Next character should be delimiter */
			if ( *j != ':' ){
				return JSON_PARSER_MALFORMED;
			}
			/* Skip the delimiter */
			j++;

			/* Ignore any leading spaces */
			j = _skip_spaces( j );

			/* Find length of value and check buffer provided.
	       	   Account for NULL */
			l = _value_len( j );
			if ( l+1 > val_size ){
				return JSON_PARSER_INSUFFICIENT_BUFFER;
			}

			if ( _copy_val( j, val ) == NULL ){
				return JSON_PARSER_COPY_ERROR;
			}

			break;
		}

		/* Find next pair */
		while ( *j != ',' && *j != '}' ){
			j++;
		}
		if ( *j == ',' ){
			j++;
		}
	}

	if ( *j == '}' ){
		return JSON_PARSER_ATTR_NOT_FOUND;
	}

	return JSON_PARSER_OK;
}
