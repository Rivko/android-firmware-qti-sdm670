#ifndef AEEJSONPARSER_H
#define AEEJSONPARSER_H
/*=============================================================================

FILE:          AEEJSONParser.h

SERVICES:      JSONParser Library.

DESCRIPTION:   API to parse JSON (JavaScript Object Notation) strings

===============================================================================
              Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include "AEEJSONTypes.h"
   
typedef struct JSONParser JSONParser;
typedef int JSONEnumState;
typedef int JSONID;

#define JSONID_ROOT 0   

#define JSONPARSER_SUCCESS    0
#define JSONPARSER_ERROR      0xFFFFFFFF
#define JSONPARSER_EOVERFLOW  0xFFFFFFFD
#define JSONPARSER_ENOMORE    0xFFFFFFFC
   
struct JSONParser {
   const char *  psz;
   unsigned int  uMax;
};

// Parser API   
void JSONParser_Ctor(JSONParser *pj, const char *psz, int nSize);
int  JSONParser_GetType(JSONParser *pj, JSONID id, JSONType *pjt);
int  JSONParser_GetJSON(JSONParser *pj, JSONID id, const char **ppcsz, int *pnSize);
int  JSONParser_ObjectEnumInit(JSONParser *pj, JSONID idObject,
                                      JSONEnumState *pstEnum);
int  JSONParser_ObjectNext(JSONParser *pj, JSONEnumState *pst,
                                  JSONID *pidKey, JSONID *pidValue);
int  JSONParser_ArrayEnumInit (JSONParser *pj, JSONID idArray,
                                      JSONEnumState *pstEnum);
int  JSONParser_ArrayNext (JSONParser *pj, JSONEnumState *pst, JSONID *pidValue);
int  JSONParser_ObjectLookup(JSONParser *pj, JSONID idObject,
                                    const char *pszName, int nLen,
                                    JSONID *pidKey, JSONID *pidValue);
int  JSONParser_ArrayLookup(JSONParser *pj, JSONID idArray,
                                   int nIndex, JSONID *pidValue);
int  JSONParser_Lookup(JSONParser *pj, JSONID id,
                              const char *szPath, int nLen, JSONID *pValID);
int  JSONParser_GetString(JSONParser *pj, JSONID id,
                                 char *psz, int nSize, int *pnReq);
int  JSONParser_GetInt (JSONParser *pj, JSONID id, int *pi);
int  JSONParser_GetUInt(JSONParser *pj, JSONID id, unsigned int *pu);
int  JSONParser_GetDouble(JSONParser *pj, JSONID id, double *pd);
int  JSONParser_GetBool(JSONParser *pj, JSONID id, int *pb);
int  JSONParser_GetNull(JSONParser *pj, JSONID id);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*==============================================================================
INTERFACES DOCUMENTATION
================================================================================
JSONParser Interface

Header File:
   AEEJSONParser.h

Description:
   This library provides a set of functions to parse JSON data.
   The parsing and accessing functions provide a simple and generic mechanism
   which requires no memory allocation.
   
   A JSONID (or simply ID) refer to a specific value within JSON data and identifies
   a particular JSONType (or just type) value. Given a JSONID, GetType() returns
   its type.

   The root of JSON data is represented by the special JSONID defined as JSONID_ROOT.
   Use this as the jumping point for looking up other IDs.
   
   Once the parser is constructed using Ctor(), one of the Lookup() methods
   can be used to query the nested JSON data. EnumInit() and Next() methods provide
   a way to enumerate arrays and objects to retrieve their member IDs.
   
   Relevant JSONIDs can then be passed to one of the GetXXX() methods to retrieve
   their underlying values.

   Use GetType() to find out the JSONType at a particular JSONID.

   A JSONParser can be declared on the stack for quick parsing needs.
   
See Also:
   None

=======================================================================

JSONParser_Ctor()

Description:
   Initializes the JSONParser structure with the provided JSON text string
   and the size.  If size is 0, then string is considered null-terminated.

   The caller provides the JSONParser structure.  This allows this structure
   to be stack declared or dynamically declared based on the caller needs.
   Stack allocation is attractive option for quick parsing needs.

   Example:
   const char *szJSON[] = "{\"company name\" : \"Qualcomm\" }";
   JSONParser jp={0};
   JSONParser_Ctor(&jp, szJSON, 0);

Prototype:
   void   JSONParser_Ctor(JSONParser *pj, const char *psz, int nSize);
 
Parameters:
   pj    : JSON context 
   psz   : JSON data
   nSize : Length of the JSON data passed.
           Value of 0 means psz is null-terminated.

Return Value:

   None

================================================================================

JSONParser_GetType()

Description:
   Returns the type of the JSON object at given JSONID

Prototype:
   int JSONParser_GetType(JSONParser *p, JSONID id, JSONType *pjt);
 

Parameters:
   pj : JSON context 
   id : ID at which the JSONType is identified
   pjt: filled with JSONType

Return Value:

   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR

================================================================================

JSONParser_GetJSON()

Description:
   Caller supplies JSON data to construct the parser. This function makes a copy
   of this buffer at the given JSONID value upto its logical end. The ID must
   point to a valid JSONType or JSONID_ROOT.

   For example, if JSONID starts a string, then the copied buffer size is the
   size of such string.  However, if it is an array, then the size would be 
   upto the end of such array, including all its nested values.
   

Prototype:
   int JSONParser_GetJSON(JSONParser *pj, JSONID id, const char **ppcsz, int *pnSize);
 

Parameters:
   pj:     JSON context 
   id:     ID at which the JSON data is copied
   ppcsz:  filled with underlying JSON data pointer
   pnSize: size of the valid JSON data

Return Value:

   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR

================================================================================
   
JSONParser_ObjectEnumInit()

Description:
   Initializes the enumeration state (pstEnum) to point to the first pair of
   the object at the given JSON ID. 
   

Prototype:
   int   JSONParser_ObjectEnumInit(JSONParser *pj, JSONID id, JSONEnumState *pstEnum); 

Parameters:
   pj         : JSON context 
   idObject   : JSONObject ID
   pstEnum    : Enumeration state is updated on success

Return Value:
   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR

================================================================================
   
JSONParser_ObjectNext()

Description:
   Given the enumeration state, reads the key and value ids of the
   current pair and updates the enumeration state to next pair or end.

Prototype:
   int JSONParser_ObjectNext(JSONParser *pj, JSONEnumState *pst, JSONID *pidKey,
                             JSONID *pidValue);

Parameters:
   pj      : JSON context 
   pst     : current enumeration state. On success, is updated to next pair or end
   pidKey  : Key ID of the current pair
   pidValue: Value ID
   
Return Value:
   JSONPARSER_SUCCESS : success -- pidKey and pidValue are updated with relevant IDs
   JSONPARSER_ENOMORE : if no more items to enumarate
   JSONPARSER_ERROR   : for all other errors
   
================================================================================
   
JSONParser_ArrayEnumInit()

Description:
   Initializes the enumeration state (pstEnum) to point to the first value of
   the array at the given JSON ID. 

Prototype:
   int JSONParser_ArrayEnumInit (JSONParser *pj, JSONID idArray,
                                 JSONEnumState *pstEnum);

Parameters:
   pj         : JSON context 
   idArray    : JSONArray ID
   pstEnum    : Enumeration state is updated on success
   
Return Value:
   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR

================================================================================
   
JSONParser_ArrayNext()

Description:
   Given the enumeration state, reads the value id at the current state
   and updates the enumeration state to next value or end.

Prototype:
   int JSONParser_ArrayNext (JSONParser *pj, JSONEnumState *pst, JSONID *pidValue);

Parameters:
   pj      : JSON context 
   pst     : current enumeration state. On success, is updated to next value or end
   pidValue: Value ID
   
Return Value:
   JSONPARSER_SUCCESS : success -- pidValue is updated with relevant ID
   JSONPARSER_ENOMORE : if no more items to enumarate
   JSONPARSER_ERROR   : for all other errors
   
================================================================================
   
JSONParser_ObjectLookup()

Description:
   Given an Object ID and a key name, finds the matching key and if found,
   updates the key and value IDs

Prototype:
   int   JSONParser_ObjectLookup(JSONParser *pj, JSONID idObject,
                           const char *pszName, int nLen,
                           JSONID *pidKey, JSONID *pidValue);
                           
Parameters:
   pj       : JSON context 
   idObject : JSONObject ID
   pszName  : JSON string to be looked up
   pidKey   : Key ID of the current pair
   pidValue : Value ID
   
Return Value:
   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR
   On success, pidKey and pidValue are updated with relevant IDs
=========================================================================================
   
JSONParser_ArrayLookup()

Description:
   Given an array ID and the 0-based index, finds the corresponding value
   and if found, updates the value ID

Prototype:
   int JSONParser_ArrayLookup(JSONParser *pj, JSONID idArray, int index,
                              JSONID *pidValue);
                           
Parameters:
   pj       : JSON context 
   idArray  : Array ID
   nIndex   : 0-based array index to lookup the value
   pidValue : Value ID
   
Return Value:
   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR
   On success, pidValue is updated with relevant ID

================================================================================
JSONParser_Lookup()

Description:
   The function provides a generic mechanism to access any JSON value
   using the string format described below.

   Format uses period (".") as the separator and hence strings with periods
   cannot use this function for lookup.  Format is divided into multiple
   segments with each segment except last forming an hierarchy of containers.
   The last segment identifies the leaf node and for objects this would be a
   key and for arrays an index number.

   Examples:
   JSON: { "Address" : {"Street Name": {"Name": "Main Blvd", "Number": 1111 }}}
   Path: "Address.Street Name.Number"
   Retrieves: 1111
   
   JSON: [0, 1, { "x" : 234.45} ]
   Path: "2.x"
   Retrieves: 234.45

   JSON: { "auth": { "options" : ["rc4", "aes"] } }
   Path: "auth.options.0"
   Retrieves: "rc4"

Prototype:
   int JSONParser_Lookup(JSONParser *pj, JSONID id, const char *szPath, int nLen,
                     JSONID *pValID);
                           
Parameters:
   pj       : JSON context 
   id       : JSONObject or JSONArray ID
   szPath   : string parsed to identify the value location
   nLen     : length of the string
   pValID : Value ID
   
Return Value:
   JSONPARSER_SUCCESS on success, otherwise JSONPARSER_ERROR
   On success, pValID is updated with relevant ID

================================================================================
JSONParser_GetString()

Description:
   Given a JSONString, retrieve the string value.

Prototype:
   int JSONParser_GetString(JSONParser *pj, JSONID id,
                         char *psz, int nSize, int *pnReq);
                           
Parameters:
   pj    :    JSON context 
   id    :    JSONString ID
   psz   :    filled with string
   nSize :    size of the psz or 0 to query for the size
   pnReq :    on return, contains the size of a buffer necessary to hold the full
              string and terminating NULL character. If nSize < *pnReq, the string
              has been truncated to fit into the buffer that was passed.
          
   
Return Value:
   JSONPARSER_SUCCESS : on success. Check *pnReq to make sure string is
                        not truncated
   otherwise JSONPARSER_ERROR

================================================================================
JSONParser_GetInt()

Description:
   Given JSONNumber, retrieve its integer value.  The underlying number must be
   an integer in the range of C int data type

Prototype:
   int  JSONParser_GetInt (JSONParser *pj, JSONID id, int *pi);
                        
Parameters:
   pj:       JSON context 
   id:       JSONNumber ID
   pi:       filled with integer value at id
   
Return Value:
   JSONPARSER_SUCCESS    : on success
   JSONPARSER_EOVERFLOW  : if number is out of range of C "int" type
   otherwise JSONPARSER_ERROR

================================================================================
JSONParser_GetUInt()

Description:
   Given JSONNumber, retrieve its unsigned integer value.  The underlying number
   must be an unsigned integer in the range of C "unsigned int" data type

Prototype:
   int JSONParser_GetUInt(JSONParser *pj, JSONID id, unsigned int *pu);
                        
Parameters:
   pj:       JSON context 
   id:       JSONNumber ID
   pu:       filled with unsigned integer value at id 
   
Return Value:
   JSONPARSER_SUCCESS    : on success
   JSONPARSER_EOVERFLOW  : if number is out of range of C "unsigned int" type
   otherwise JSONPARSER_ERROR

================================================================================
JSONParser_GetDouble()
   Given JSONNumber, retrieve its floating point value. 

Description:

Prototype:
   int JSONParser_GetDouble(JSONParser *pj, JSONID id, double *pd);
                        
Parameters:
   pj:       JSON context 
   id:       JSONNumber ID
   pd:       filled with double value at id 
   
Return Value:
   JSONPARSER_SUCCESS    : on success
   JSONPARSER_EOVERFLOW  : if number is out of range of C "double" type
   otherwise JSONPARSER_ERROR

================================================================================
JSONParser_GetBool()

Description:
   Given JSONNumber, retrieve its boolean value. 

Prototype:
   int JSONParser_GetBool(JSONParser *pj, JSONID id, int *pb);
                        
Parameters:
   pj:       JSON context 
   id:       JSBool ID
   pb:       filled with boolean value at id
             zero represents false
             non zero represents true
   
Return Value:
   JSONPARSER_SUCCESS:   on success,
   otherwise JSONPARSER_ERROR

================================================================================
JSONParser_GetNull()

Description:
   Given a JSONID, returns success if its a null value

Prototype:
   int JSONParser_GetNull(JSONParser *pj, JSONID id);
                        
Parameters:
   pj:       JSON context 
   id:       JSONNull location
   
Return Value:
   JSONPARSER_SUCCESS:  if there is a null value at id
   otherwise JSONPARSER_ERROR

=============================================================================
   ERRORS DOCUMENTATION
==============================================================================
JSONParser error codes

Definition:
   JSON Parser error codes

Error                         Code                    Description

JSONPARSER_SUCCESS            0                       Operation is successful
JSONPARSER_ERROR              0xFFFFFFFF              Error in operation
JSONPARSER_EOVERFLOW          0xFFFFFFFD              exceeded the max value
JSONPARSER_ENOMORE            0xFFFFFFFC              No more value

===============================================================================*/
#endif
