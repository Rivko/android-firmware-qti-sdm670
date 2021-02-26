#ifndef AEEJSONTYPES_H
#define AEEJSONTYPES_H
/*=============================================================================

FILE:          AEEJSONTypes.h

SERVICES:      Common JSON Types

DESCRIPTION:   Common JSON (JavaScript Object Notation) types

===============================================================================
              Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


typedef enum {
   JSONObject,
   JSONArray,
   JSONNumber,
   JSONString,
   JSONBool,
   JSONNull,
   JSONUnknown=0x7FFFFFFF
} JSONType;


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*==============================================================================
DATA STRUCTURES DOCUMENTATION
================================================================================
JSONType

Description:
   This enum type represents various JSON value types

Definition:
   typedef enum JSONType;

Members:
   JSONObject  : refers to a JSON Object type
   JSONArray   : refers to a JSON JSONArray type
   JSONNumber  : refers to a JSON JSONNumber type
   JSONString  : refers to a JSON JSONString type
   JSONBool    : refers to a JSON JSONBool type
   JSONNull    : refers to a JSON JSONNull type
   JSONUnknown : refers to a JSON JSONUnknown type

See Also:
   None
===============================================================================*/
#endif
