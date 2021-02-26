/*=============================================================================
 
  File: parserutil.c
  
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/MDPLib/parserutil.c#1 $$
  
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 
=============================================================================*/ 

#include "xmlparser.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
 * Parser Helper Function
 */


/* XML_Strcpy()
 *  - Safe, os independent string copy
 */
void XML_Strcpy(xmlchar *pDst, const xmlchar *pSrc)
{
    uint32 i = 0;
    for (i=0;i<(XML_MAX_STRING_LENGTH-1);i++)
    {
        if (XML_TAG_NULL != pSrc[i])
        {
            pDst[i] = pSrc[i];
        }
        else
        {
            pDst[i] = XML_TAG_NULL;
            return;
        }
    }
    // String has exceeded maxium number of characters, just null terminate and return;
    pDst[i] = XML_TAG_NULL;
}



/* XML_Strlen()
 *  - Safe, os independent string copy
 */
uint32 XML_Strlen(const xmlchar *pStr)
{
    uint32 i = 0;

    for (i=0;i<(XML_MAX_STRING_LENGTH-1);i++)
    {
        if (XML_TAG_NULL == pStr[i])
            break;
    }

    return i;
}

/* XML_StrIsWord()
 *  - Safe, os independent word find (search for a word within a string)
 *  - return 1 if there is a match
 */
int32 XML_StrIsWord(const xmlchar *pWord, const xmlchar *pSubstring, int iWordLength, int iDataAvailable)
{
    int32 iResult = 0;

    // Only bother if we can fit our word in the data buffer
    if (iDataAvailable >= iWordLength)
    {
        int32 iCharMatch = 0;

        for (iCharMatch=0;iCharMatch<iWordLength;iCharMatch++)
        {
            if (!XML_COMPARE_CHAR(pWord[iCharMatch], pSubstring[iCharMatch]))
                break;
        }

        // Ensure we have a complete word
        if ((iCharMatch==iWordLength) &&
            ((iDataAvailable == iWordLength) || 
             (XML_IS_WHITESPACE(pSubstring[iCharMatch]))))
        {
            iResult++;
        }

    }

    return iResult;
}


/* XML_GetTagWord()
 *  - Safe, os independent tag parser
 *  - return the length of the tag word
 */
int32 XML_GetTagWord(xmlchar **pTag, const xmlchar *pXmlData, int iDataAvailable)
{
    int32 iResult = 0;
    int32 iDataLength = iDataAvailable;
    xmlchar *pData = (xmlchar*) pXmlData;

    // Skip white spacing
    while ((iDataLength>0) && (XML_IS_WHITESPACE(pData[0])))
    {
        pData++;
        iDataLength--;
    }
    // We've found a non-whitespace and still have data in the buffer
    if (iDataLength)
    {
        // Return the start of the tag
        *pTag = pData;

        // Count how many character in the tag
        while ((iDataLength>0) && (XML_IS_ALPHANUMERIC(pData[0])))
        {
            pData++;
            iDataLength--;
            iResult++;
        }
    }

    return iResult;
}


/* XML_Strcmp()
 *  - Safe, os independent string compare
 *  - return 1 if there is a match
 */
int32 XML_Strcmp(const xmlchar *pWord, const xmlchar *pSubstring, int iWordLength, int iDataAvailable)
{
    int32 iResult = 0;

    // Only bother if we can fit our word in the data buffer
    if (iDataAvailable >= iWordLength)
    {
        int32 iCharMatch = 0;

        for (iCharMatch=0;iCharMatch<iWordLength;iCharMatch++)
        {
            if (!XML_COMPARE_CHAR(pWord[iCharMatch], pSubstring[iCharMatch]))
                break;
        }

        // Ensure we have a complete word
        if (iCharMatch==iWordLength) 
        {
            iResult++;
        }

    }

    return iResult;
}



/* XML_Strncat()
 *  - Safe, os independent string concatenation
 */
void XML_Strncat(xmlchar *pDst, const xmlchar *pSrc, uint32 uMaxLen)
{
    uint32 i      = 0;
    uint32 c      = 0;
    uint32 uStart = 0;

    for (i=0;i<(XML_MAX_STRING_LENGTH-1);i++)
    {
        if (XML_TAG_NULL == pDst[i])
        {
            uStart = i;
            break;
        }
    }

    // At this point we've either found the end of the pDst string or we've exceeded the maximum string length. 
    // Append the sources to the destination
    for (i=uStart;i<(XML_MAX_STRING_LENGTH-1);i++)
    {
        if ( c >=uMaxLen )
        {
            break;
        }
        else if( XML_TAG_NULL == pSrc[c])
        {
            break;
        }
        else
        {
            pDst[i] = pSrc[c++];
        }
    }

    // Append the null terminated character
    pDst[i] = XML_TAG_NULL;
}


/* XML_Substring()
 *  - Return's a copied substring of an input string
 */
xmlchar *XML_Substring(xmlchar *pString, uint32 uLen)
{
    static xmlchar subString[XML_MAX_STRING_LENGTH];
    uint32 maxString = (uLen<(XML_MAX_STRING_LENGTH-1))?uLen:(XML_MAX_STRING_LENGTH-1);
    uint32 i;
    
    for (i=0;i<maxString;i++)
    {
        subString[i] = pString[i];

        if (XML_TAG_NULL == pString[i])
        {
            break;
        }
    }
    subString[i] = XML_TAG_NULL;

    return &subString[0];
}


#ifdef __cplusplus
  }
#endif
