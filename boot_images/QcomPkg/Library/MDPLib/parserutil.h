#ifndef _PARSER_UTIL_H_
#define _PARSER_UTIL_H_
/*=============================================================================
 
  File: parserutil.h
  
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/MDPLib/parserutil.h#1 $$
  
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 
=============================================================================*/ 

#include "MDPTypes.h"


#ifndef _BOOL32_DEFINED
typedef  unsigned long int  bool32;        /* Boolean value type. */
#define _BOOL32_DEFINED
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x)          (void)(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* XML_Strcpy()
 *  - Safe, os independent string copy
 */
void XML_Strcpy(xmlchar *pDst, const xmlchar *pSrc);

/* XML_Strlen()
 *  - Safe, os independent string copy
 */
uint32 XML_Strlen(const xmlchar *pStr);

/* XML_StrIsWord()
 *  - Safe, os independent word find (search for a word within a string)
 *  - return 1 if there is a match
 */
int32 XML_StrIsWord(const xmlchar *pWord, const xmlchar *pSubstring, int iWordLength, int iDataAvailable);


/* XML_GetTagWord()
 *  - Safe, os independent tag parser
 *  - return the length of the tag word
 */
int32 XML_GetTagWord(xmlchar **pTag, const xmlchar *pXmlData, int iDataAvailable);


/* XML_Strcmp()
 *  - Safe, os independent string compare
 *  - return 1 if there is a match
 */
int32 XML_Strcmp(const xmlchar *pWord, const xmlchar *pSubstring, int iWordLength, int iDataAvailable);



/* XML_Strncat()
 *  - Safe, os independent string concatenation
 */
void XML_Strncat(xmlchar *pDst, const xmlchar *pSrc, uint32 uMaxLen);


/* XML_Substring()
 *  - Return's a copied substring of an input string
 */
xmlchar *XML_Substring(xmlchar *pString, uint32 uLen);


#ifdef __cplusplus
}
#endif

#endif // _PARSER_UTIL_H_
