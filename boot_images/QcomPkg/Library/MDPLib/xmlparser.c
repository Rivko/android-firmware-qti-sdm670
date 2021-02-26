/*=============================================================================
 
  File: xmlparser.c
  
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/MDPLib/xmlparser.c#1 $$
  
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 
=============================================================================*/ 

#include "xmlparser.h"
#include "parserutil.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Maximum number of tag we will search and support
 */
#define XML_PARSER_MAX_TAGS                 1024

/* Maximum number of decimal digits we can parse
 */
#define XML_PARSER_MAX_DECIMAL_DIGITS       10

/* Maximum number of decimal digits we can parse
 */
#define XML_PARSER_MAX_HEX_DIGITS            8

/* Maximum number of GUID fields we can processes
 */
#define XML_PARSER_MAX_GUID_FIELDS           11

/* Function Prototypes 
 */
static uint32 XML_ParserInit(XML_ParserContextType *pParserCtx);

static uint32 XML_ParseTags(XML_ParserContextType *pParserCtx);

/* Helper Function Prototypes 
 */
static XML_ParserStateType XML_FindNamedTag(const xmlchar *pTagName, int32 iTagLength, xmlchar **pXmlData, int32 *pXmlDataLength);

static XML_ParserStateType XML_FindTag(xmlchar **pXmlData, int32 *pXmlDataLength, XML_ParserContextType *pParserCtx);

static uint32 XML_FindTagLength(const xmlchar *pTagName, int32 iTagLength, uint32 *pFieldLength, xmlchar *pXmlData, int32 iDataLength);

static XML_ParserStateType XML_ParseField(XML_ParserContextType *pParserCtx, xmlchar *pFieldData, uint32 uFieldLength);

static bool32 XML_FindGroupTag(const xmlchar *pGroupName, xmlchar **pXmlData, int32 *pXmlDataLength);

static uint32 XML_FindGroupTagLength(xmlchar *pXmlData, int32 uXmlDataLength);


/* Field Parser Helper Functions 
 */
static uint32 XML_ParseFieldString(void *pOutput, uint32 uMaxOutputLen, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldInteger(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldBoolean(void *pOutput, uint32 uMaxOutputLen, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldBinary(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldGUID(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldIntList(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen);

static uint32 XML_ParseFieldHexList(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen);

/* Helper parsing functions
 */
static uint32 ParseHexValue(xmlchar *pInput, uint32 uInputLen, void *pOutput, uint32 uOutputSize);

static uint32 ParseDecimalValue(xmlchar *pInput, uint32 uInputLen, void *pOutput, uint32 uOutputSize);
 
/* Function table pointers
 */
static const XML_ParserFnType xmlParserFn[XML_STATE_MAX] = {
    &XML_ParserInit,  // XML_STATE_INIT,
    &XML_ParseTags,   // XML_STATE_TAG_SEARCH,
    NULL, // XML_STATE_ERROR,
    NULL, // XML_STATE_MAX
};

static const XML_FieldParserFnType xmlFieldParserFn[XML_TAG_DATATYPE_MAX] = {
  NULL,                     // XML_TAG_DATATYPE_NONE,
  &XML_ParseFieldString,    // XML_TAG_DATATYPE_STRING,
  &XML_ParseFieldInteger,   // XML_TAG_DATATYPE_INTEGER,
  NULL,                     // XML_TAG_DATATYPE_FLOAT,
  &XML_ParseFieldBoolean,   // XML_TAG_DATATYPE_BOOLEAN,
  &XML_ParseFieldBinary,    // XML_TAG_DATATYPE_BINARY
  &XML_ParseFieldGUID,      // XML_TAG_DATATYPE_GUID
  &XML_ParseFieldIntList,   // XML_TAG_DATATYPE_INTEGER_LIST
  &XML_ParseFieldHexList    // XML_TAG_DATATYPE_HEX_LIST
};


/* public function
 */

/* Update parsed XML output data within a XML tag list for given XML Tag 
 */
uint32 XML_UpdateTag( XML_TagDefType     *pXmlTagList,
                      uint32             uMaxTags,
                      uint8             *pXMLTag,
                      uint32             uXMLTagLength,
                      void              *pXMLDataField,
                      uint16             uXMLDataFieldLength)
{
  uint32  result  = XML_PARSER_STATUS_BADPARAM;

  if ((NULL == pXmlTagList) ||
      (NULL == pXMLTag)     ||
      (NULL == pXMLDataField))
  {
    result  = XML_PARSER_STATUS_BADPARAM;
  }
  else
  {
    uint32  i       = 0;

    for (i=0; i<uMaxTags; i++ )
    {
      if (XML_Strcmp((const xmlchar *)pXmlTagList[i].uXMLTag, (const xmlchar *)pXMLTag, pXmlTagList[i].uXMLTagLength, uXMLTagLength))
      {
        pXmlTagList[i].pXMLDataField       = pXMLDataField;
        pXmlTagList[i].uXMLDataFieldLength = uXMLDataFieldLength;
        result                             = XML_PARSER_STATUS_OK;
        break;
      }
    }
  }

  return result;
}

/* Main XML parser function
 */
uint32 XML_Parser(void              *pXmlData,
                  uint32             uDataLength,
                  XML_TagDefType    *pXmlTagList,
                  uint32             uMaxTags)
{
    uint32 result = XML_PARSER_STATUS_OK;


    /* Simple parameters checks */
    if ((NULL == pXmlData) ||
        (NULL == pXmlTagList) ||
        (0==uDataLength))
    {
        result = XML_PARSER_STATUS_BADPARAM;
    }
    else
    {
        XML_ParserContextType    parserCtx;

        // Initialize parser context
        parserCtx.eParserState = XML_STATE_INIT;
        parserCtx.pDataStartPtr = (xmlchar*)pXmlData;
        parserCtx.iDataAvailable = uDataLength;
        parserCtx.pXmlTagList = pXmlTagList;
        parserCtx.uXmlTagListLength = uMaxTags;

        // Continue to parse as long as data remains 
        while ((parserCtx.iDataAvailable>0) &&
               ((parserCtx.eParserState != XML_STATE_ERROR) &&
                (parserCtx.eParserState != XML_STATE_DONE)))
        {
            // Call the function for this specific state
            xmlParserFn[parserCtx.eParserState](&parserCtx);
        }
    }

    return result;
}


/* Parser function to part the XML test starting at a particular group name
 * ex. <Group id="Group Name">
 * 
 */
uint32 XML_ParserGroup(xmlchar        *pGroupName,
                       void           *pXmlData,
                       uint32          uDataLength,
                       XML_TagDefType *pXmlTagList,
                       uint32          uMaxTags)
{
    void  *pXmlGroupStart  = pXmlData;
    uint32 uXmlGroupLength = uDataLength;

    if (NULL != pGroupName)
    {
        xmlchar *pData = (xmlchar*)pXmlData;
        int32   iDataLength = uDataLength;

        // Search for a group tag and determine the length
        if (TRUE == XML_FindGroupTag(pGroupName, &pData, &iDataLength))
        {
            uint32 uGroupLength = XML_FindGroupTagLength(pData, iDataLength);

            // Found the length of this group, restruct the data to only data within this group block
            if (uGroupLength) 
            {
                iDataLength = uGroupLength;
            }

            pXmlGroupStart = pData;
            uXmlGroupLength = iDataLength;
        }
    }

    // Find start the normal parser path with the updated pointers
    return XML_Parser(pXmlGroupStart, uXmlGroupLength, pXmlTagList, uMaxTags);
}



/* XML Parser utility to parse our fields from a string, 
 * 
 *   dataA,   dataB,  dataC
 *   -------------------
 *   field0, field1, field2
 */
uint32 XML_ParseCSV(void *pData, uint32 uDataLength, uint32 uFieldNum, XML_TagDataType eDataType, void *pOuputData, uint32 uOutputSize)
{
    uint32 result = XML_PARSER_STATUS_OK;

    /* Simple parameters checks */
    if ((NULL == pData) ||
        (NULL == pOuputData) ||
        (0==uDataLength))
    {
        result = XML_PARSER_STATUS_BADPARAM;
    }
    else
    {
        xmlchar *pStr = (xmlchar*)pData;
        uint32 uIndex;
        uint32 uLastCSVIndex = 0;
        uint32 uNextCSVIndex = 0;
        uint32 uFieldIndex = 0;

        // Search for the ',' character to determine the start and end of the field.
        for (uIndex=0;uIndex<(uDataLength-1);uIndex++)
        {
            if ((XML_TAG_CSV == pStr[uIndex+1]) || (XML_TAG_NULL == pStr[uIndex+1]))
            {
                // Mark a end of field
                uFieldIndex++;
                // Mark this as the next end of the index
                uNextCSVIndex = uIndex+2;
                // Skip the comma
                uIndex++;
                // Increment the field
                if ((uFieldIndex > uFieldNum) || (XML_TAG_NULL == pStr[uIndex+1]))
                {
                    break;
                }
                else
                {
                    // Move up the start and end index
                    uLastCSVIndex = uNextCSVIndex;
                }
            }
        }

        // If the field location is found
        if ((uFieldIndex> uFieldNum) && (uNextCSVIndex>uLastCSVIndex))
        {
            uint32 uFieldLength = uNextCSVIndex - uLastCSVIndex-1;

            if (0==xmlFieldParserFn[eDataType](pOuputData, uOutputSize, (void*)&pStr[uLastCSVIndex], uFieldLength))
            {
                // Parsing of the field failed
                result = XML_PARSER_STATUS_BADPARAM;
            }
        }
        else
        {
            // Field is out of bounds cannot find the proper field
            result = XML_PARSER_STATUS_BADPARAM;
        }
    }

    return result;
}



/* XML_ParserInit()
 *  - Handle initial setup of the parser context
 *
 *  Next state: XML_STATE_HEADER_SEARCH - on success
 *              XML_STATE_ERROR - on error
 *              XML_STATE_DONE - empty or non-xml buffer
 *
 */
static uint32 XML_ParserInit(XML_ParserContextType *pParserCtx)
{
    uint32 result = XML_PARSER_STATUS_OK;
    
    // Go to the initialization state we have have valid tags 
    if (pParserCtx->uXmlTagListLength)
    {
        XML_ParserStateType eParserState;
        uint32              iTagLength = 0;
        xmlchar            *pData = pParserCtx->pDataStartPtr;
        int32               iDataLength = pParserCtx->iDataAvailable;

        // Find the header tags <?xml>
        eParserState = XML_FindNamedTag((xmlchar*)XML_HEADER_TAG, XML_STRLEN(XML_HEADER_TAG), &pData, &iDataLength);

        // If we are successful, find the end of the tag
        if (XML_STATE_TAG_FOUND == eParserState)
        {
            // Determine how long this tag is.
            iTagLength = XML_FindTagLength((xmlchar*)XML_HEADER_TAG, XML_STRLEN(XML_HEADER_TAG), NULL, pData, iDataLength);
        }

        if (iTagLength > 0)
        {
          uint32 uIndex;

          // Move the data pointer up (exclude the tag start character since that is already parsed off)
          pParserCtx->pCurrentDataPtr = pData + iTagLength;
          pParserCtx->iDataAvailable = iDataLength - iTagLength;

          // Reset the tag information and parser state
          pParserCtx->pCurrentTag = pData;
          pParserCtx->uCurrentTagLength = iTagLength;
          pParserCtx->eParserState = XML_STATE_TAG_SEARCH;

          // Fill flagged tags with default values
          for (uIndex = 0; uIndex < pParserCtx->uXmlTagListLength; uIndex++)
          {
            if (XML_TAG_FLAGS_FILL & pParserCtx->pXmlTagList[uIndex].uTagFlags)
            {
              uint8 *pData = (uint8*)pParserCtx->pXmlTagList[uIndex].pXMLDataField;
              uint32 uCount;

              for (uCount = 0; uCount < pParserCtx->pXmlTagList[uIndex].uXMLDataFieldLength; uCount++)
              {
                *pData++ = pParserCtx->pXmlTagList[uIndex].uDefaultFillValue;
              }
            }
          }

        }
        else
        {
          XML_ERROR_MSG("Error: XML failed to locate <?xml version='xx' encoding='xx'?> header tag");
        
          pParserCtx->eParserState = XML_STATE_ERROR;
        }

    }
    else
    {
        pParserCtx->eParserState = XML_STATE_DONE;
    }

    return result;
}

/*********************************************************************************************************
 * Local Helper Function
 */


/* XML_ParseTags()
 *  - Search and parse tags found in a buffer
 *
 *  Next state: XML_STATE_HEADER_SEARCH - on sucess
 *              XML_STATE_DONE - if error
 *
 */
static uint32 XML_ParseTags(XML_ParserContextType *pParserCtx)
{
    uint32 result = XML_PARSER_STATUS_OK;
    XML_ParserStateType eParserState = pParserCtx->eParserState; 
    xmlchar *pData = pParserCtx->pCurrentDataPtr;
    int32   iDataAvailable = pParserCtx->iDataAvailable;

    while ((iDataAvailable>0) && (XML_STATE_ERROR != eParserState))
    {
        // Find the first tag
        eParserState = XML_FindTag(&pData, &iDataAvailable, pParserCtx);

        if (XML_STATE_TAG_FOUND == eParserState)
        {
            uint32 uBlockLength; 
            uint32 uFieldLength;

            // Look up to see if this tag exist in our database
            uBlockLength = XML_FindTagLength(pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength, &uFieldLength, pData, iDataAvailable); 

            // Parse the tag
            if (uBlockLength)
            {
                XML_ParseField(pParserCtx, pData, uFieldLength);

                // Jump to next tag
                pData += uBlockLength;
                iDataAvailable -= uBlockLength;
            }
        }
    }

    // Return new tag information and remaining data
    pParserCtx->pCurrentDataPtr = pData;
    pParserCtx->iDataAvailable = iDataAvailable;

    return result;
}




/* XML_FindTagLength()
 *  - Return the total number of bytes from tag start to tag end
 *  ex. <tag>fields</tag>
 */
static uint32 XML_FindTagLength(const xmlchar *pTagName, int32 iTagLength, uint32 *pFieldLength, xmlchar *pXmlData, int32 iDataLength)
{
    uint32 uLength = 0;

    // Ensure we are actually in a valid tag
    if (NULL != pTagName)
    {
        int32   iDataAvailable = iDataLength;

        xmlchar sEndTag[XML_MAX_STRING_LENGTH];
        uint32  uEndTagLength;
        xmlchar *pData = pXmlData;
        int32   iCommentField = 0;


        XML_CLEAR_STRING(sEndTag);

        // If this is a header tag look for header tag end
        if (XML_StrIsWord((const xmlchar*)XML_HEADER_TAG, pTagName, iTagLength, iDataAvailable))
        {
            XML_Strncat(sEndTag, (const xmlchar*)XML_HEADER_TAG_END, XML_STRLEN(XML_HEADER_TAG_END));
        }
        else if (XML_StrIsWord((const xmlchar*)XML_COMMENT_TAG, pTagName, iTagLength, iDataAvailable))
        {
            // Handle comments
            XML_Strncat(sEndTag, (const xmlchar*)XML_COMMENT_END, XML_STRLEN(XML_COMMENT_END));
            iCommentField = 1;
        }
        else
        {
            XML_Strncat(sEndTag, (const xmlchar*)XML_TAG_EXIT_START, XML_STRLEN(XML_TAG_EXIT_START));
            XML_Strncat(sEndTag, pTagName, iTagLength);
            XML_Strncat(sEndTag, (const xmlchar*)XML_TAG_ENTRY_END, XML_STRLEN(XML_TAG_ENTRY_END));
        }

        uEndTagLength = XML_Strlen(sEndTag);

        // Build the end tag we are searching for
        while (iDataAvailable)
        {
            // Match the first character before calling the function to match the word.
            // The first cases handles the <?xml ?> cases and <tag> field </tag> cases
            if ((pData[0] == sEndTag[0]) &&
                (XML_Strcmp(sEndTag, pData, uEndTagLength, iDataAvailable)))
            {
                // Find the difference between where the tag started and where it ended (plus the tag itself)
                uLength = (uint32)((uintPtr)pData - (uintPtr)pXmlData) + uEndTagLength;

                // Return the field length for this tag (if necessary)
                if (NULL != pFieldLength)
                {
                    // For fields that are within comments, simply set field length as zero and avoid parsing any data
                    if (iCommentField)
                    {
                        *pFieldLength = 0;    
                    }
                    else
                    {
                        *pFieldLength = (uint32)((uintPtr)pData - (uintPtr)pXmlData);
                    }
                    
                }
                break;
            }
          
            pData++;
            iDataAvailable--;
        }

        if (!uLength)
        {
            XML_INFO_MSG("Warning: Cannot find end of tag [%s], tag ignored", sEndTag);
        }
    }


    return uLength;
}


/* XML_FindGroupTag()
 *  - Search for a named tag
 */
static bool32 XML_FindGroupTag(const xmlchar *pGroupName, xmlchar **pXmlData, int32 *pXmlDataLength)
{
    bool32    bGroupFound = FALSE;
    int32     iDataAvailable = *pXmlDataLength;
    xmlchar   *pData = *pXmlData;
    uint32    uGroupNameLength = XML_Strlen(pGroupName);


    // Keep loop until we run out of data or we change states
    while ((iDataAvailable>0) && (FALSE == bGroupFound))
    {
        // Search for tag entry
        // Check the first character
        if (XML_COMPARE_CHAR(XML_TAG_ENTRY_START[0], pData[0]))
        {
            // Move to the next word/character
            pData++;
            iDataAvailable--;
            
            // Search for the group tag name <Group id="name">  </Group.
            //                                      ^^^^^^^^^ GroupID
            if (XML_Strcmp((const xmlchar *)XML_GROUP_TAG, pData, XML_STRLEN(XML_GROUP_TAG), iDataAvailable))
            {
                xmlchar *pGroupIdStart;

                // Ensure this tag has an end parse till the end of the
                //  ex. tag <tag   >
                //                 ^ Tagend
                pData += XML_STRLEN(XML_GROUP_TAG);
                iDataAvailable -= XML_STRLEN(XML_GROUP_TAG);

                // Mark the start of the group fields
                pGroupIdStart = pData;

                // Search for end of tag character
                while ((iDataAvailable>0) && (!XML_COMPARE_CHAR(XML_TAG_ENTRY_END[0], pData[0])))
                {
                    pData++;
                    iDataAvailable--;
                }

                // Ensure we found a ending group tag
                if (XML_COMPARE_CHAR(XML_TAG_ENTRY_END[0], pData[0]))
                {
                    uint32   uGroupIdLength = (uint32)((uintPtr)pData - (uintPtr)pGroupIdStart);

                    // Move to pointer past the '>' tag
                    pData++;
                    iDataAvailable--;

                    // Extract out the group name from this string
                    if ((uGroupIdLength > 0) && (uGroupIdLength > uGroupNameLength))
                    {
                        uint32 i;

                        // Search for the name within the substring, we only need to search a subset of the GroupData
                        for (i=0;i<(uGroupIdLength-uGroupNameLength);i++)
                        {
                            if (XML_Strcmp(&pGroupIdStart[i], pGroupName, uGroupNameLength, iDataAvailable))
                            {
                                // Found the group name, return
                                *pXmlData = pData;
                                *pXmlDataLength = iDataAvailable;
                                bGroupFound = TRUE;
                                break;
                            }
                        }

                    }
                }
            }
        }
        else
        {
            // Move to the next word/character
            pData++;
            iDataAvailable--;
        }
    }

    return bGroupFound;
}



/* XML_FindGroupLength()
 *  - Search for the </Group> Tag, also handles if there are nested <Group> </Group> tags within the group
 */
static uint32 XML_FindGroupTagLength(xmlchar *pXmlData, int32 uXmlDataLength)
{
    int32     iDataAvailable    = uXmlDataLength;
    xmlchar   *pData            = pXmlData;
    uint32    uNestedGroupCount = 0;
    uint32    uGroupLength      = 0;

    while (iDataAvailable>0)
    {
        // Search for tag entry
        // Check the first character
        if (XML_COMPARE_CHAR(XML_TAG_ENTRY_START[0], pData[0]))
        {
            // Move to the next word/character
            pData++;
            iDataAvailable--;
            
            // Check for </
            if ((XML_COMPARE_CHAR(XML_TAG_EXIT_START[1], pData[0])) &&
                (XML_Strcmp((const xmlchar *)XML_GROUP_TAG, &pData[1], XML_STRLEN(XML_GROUP_TAG), iDataAvailable)))
            {
                // Find the trailing '>
                while ((iDataAvailable>0) && (!XML_COMPARE_CHAR(XML_TAG_ENTRY_END[0], pData[0])))
                {
                    pData++;
                    iDataAvailable--;
                }

                // If we've found the closing group bracked </Group>
                if (XML_COMPARE_CHAR(XML_TAG_ENTRY_END[0], pData[0]))
                {
                    pData++;
                    iDataAvailable--;

                    // Check for nested groups
                    if (0==uNestedGroupCount)
                    {
                        //Returne the number of bytes consumed
                        uGroupLength = uXmlDataLength - iDataAvailable;
                        break;
                    }
                    else
                    {
                        uNestedGroupCount--;
                    }
                }
            }
            else if (XML_Strcmp((const xmlchar *)XML_GROUP_TAG, pData, XML_STRLEN(XML_GROUP_TAG), iDataAvailable))
            {
                // Move up the pointer
                pData += XML_STRLEN(XML_GROUP_TAG);
                iDataAvailable -= XML_STRLEN(XML_GROUP_TAG);
                // Keep track of the nested loop
                uNestedGroupCount++;
            }
        }
        else
        {
            // Move to the next word/character
            pData++;
            iDataAvailable--;
        }
    }

    return uGroupLength;
}


/*  XML_FindNamedTag()
 *  - Search for a named tag
 */
static XML_ParserStateType  XML_FindNamedTag(const xmlchar *pTagName, int32 iTagLength, xmlchar **pXmlData, int32 *pXmlDataLength)
{
    XML_ParserStateType eLocalParserState = XML_STATE_TAG_SEARCH;
    int32  iDataAvailable = *pXmlDataLength;
    xmlchar *pData = *pXmlData;

    // Keep loop until we run out of data or we change states
    while ((iDataAvailable>0) && (eLocalParserState == XML_STATE_TAG_SEARCH))
    {
        // Search for tag entry
        // Check the first character
        if (XML_COMPARE_CHAR(pTagName[0], pData[0]))
        {
            // Compare the whole word
            if (XML_StrIsWord(pTagName, pData, iTagLength, iDataAvailable))
            {   
                // return from function
                eLocalParserState = XML_STATE_TAG_FOUND;
                break;
            }
        }
        // Move to the next word/character
        pData++;
        iDataAvailable--;
    }

    // Return new tag information and remaining data
    *pXmlData = pData;
    *pXmlDataLength = iDataAvailable;


    return eLocalParserState;
}





/* XML_FindTag()
 *  - First the first valid tag
 */
static XML_ParserStateType XML_FindTag(xmlchar **pXmlData, int32 *pXmlDataLength, XML_ParserContextType *pParserCtx)
{
    XML_ParserStateType eLocalParserState = XML_STATE_TAG_SEARCH;
    int32  iDataAvailable = *pXmlDataLength;
    xmlchar *pData = *pXmlData;

    // Keep loop until we run out of data or we change states
    while ((iDataAvailable>0) && (eLocalParserState == XML_STATE_TAG_SEARCH))
    {
        // Search for tag entry
        // Check the first character
        if (XML_COMPARE_CHAR(XML_TAG_ENTRY_START[0], pData[0]))
        {
            // Move to the next word/character
            pData++;
            iDataAvailable--;

             // Check to see if this is a comment first
            if ((XML_COMPARE_CHAR(XML_COMMENT_TAG[0], pData[0])) &&
                XML_Strcmp((const xmlchar *)XML_COMMENT_TAG, pData, XML_STRLEN(XML_COMMENT_TAG), iDataAvailable))
            {
                // Update the current tag
                pParserCtx->uCurrentTagLength = XML_STRLEN(XML_COMMENT_TAG);
                pParserCtx->pCurrentTag = pData;

                // Skip to the end of the comment and return 
                pData += XML_STRLEN(XML_COMMENT_TAG);
                iDataAvailable -= XML_STRLEN(XML_COMMENT_TAG); 

                eLocalParserState = XML_STATE_TAG_FOUND;
                break;
            }
            else if ((pParserCtx->uCurrentTagLength = XML_GetTagWord(&pParserCtx->pCurrentTag, pData, iDataAvailable)) > 0)
            {   
                // Ensure this tag has an end parse till the end of the
                //  ex. tag <tag   >
                //                 ^ Tagend
                pData += pParserCtx->uCurrentTagLength;
                iDataAvailable -= pParserCtx->uCurrentTagLength;

                // Search for end of tag character
                while ((iDataAvailable>0) && (!XML_COMPARE_CHAR(XML_TAG_ENTRY_END[0], pData[0])))
                {
                    pData++;
                    iDataAvailable--;
                }

                // We should have found the end of the tag character '>', otherwise we ran out of data
                // report if the tag was found at this point.
                if (iDataAvailable)
                {
                    // Ignore the "group" keywords
                    if (XML_Strcmp((const xmlchar *)XML_GROUP_TAG, pParserCtx->pCurrentTag, XML_STRLEN(XML_GROUP_TAG), pParserCtx->uCurrentTagLength))
                    {
                        eLocalParserState = XML_STATE_TAG_SEARCH;
                    }
                    // Skip any end tags ex </xxxxxx>
                    else if ((pParserCtx->pCurrentTag[-1] == XML_TAG_EXIT_START[0]) && (pParserCtx->pCurrentTag[0] == XML_TAG_EXIT_START[1]))
                    {
                        eLocalParserState = XML_STATE_TAG_SEARCH;
                    }
                    // Check for "/>" to indicate that the tag 
                    else if (pData[-1] == XML_TAG_SHORTENTRY_END[0])
                    {
                        // Found an empty tag, no need to continue to parse this tag (there is no content)
                        eLocalParserState = XML_STATE_TAG_SEARCH;
                    }
                    else
                    {
                        eLocalParserState = XML_STATE_TAG_FOUND;
                    }

                    // Skip over the trailing '>' and return at the next character
                    pData++;
                    iDataAvailable--;
                }
                break;
            }
        }
        // Move to the next word/character
        pData++;
        iDataAvailable--;
    }

    // Return new tag information and remaining data
    *pXmlDataLength = iDataAvailable;
    *pXmlData = pData;

    return eLocalParserState;
}






/* XML_ParseField()
 *  - Parse a field in to the tag data
 */
static XML_ParserStateType XML_ParseField(XML_ParserContextType *pParserCtx, xmlchar *pFieldData, uint32 uFieldLength)
{
    XML_ParserStateType eLocalState = XML_STATE_DONE;

    if (uFieldLength)
    {
        XML_TagDefType     *pTagDefn = NULL;
        uint32              i;

        // Search for a tag match
        for (i=0;i<pParserCtx->uXmlTagListLength;i++)
        {
            // Search for the matching tag
            if ((pParserCtx->uCurrentTagLength == pParserCtx->pXmlTagList[i].uXMLTagLength) &&
                (XML_Strcmp(pParserCtx->pXmlTagList[i].uXMLTag, pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength, pParserCtx->uCurrentTagLength)))
            {
                pTagDefn = &pParserCtx->pXmlTagList[i];
                break;
            }
        }

        if (NULL == pTagDefn) 
        {
            XML_INFO_MSG("Info: No tag definition for field [%s] tag skipped", XML_Substring(pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength));
        }
        else if (NULL == xmlFieldParserFn[pTagDefn->eXMLTagDataType])
        {
            XML_INFO_MSG("Error: No parser function for field [%s]", XML_Substring(pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength));
        }
        else
        {
            XML_INFO_MSG("Info: Found valid tag [%s] type [%i]", XML_Substring(pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength), pTagDefn->eXMLTagDataType);

            if (0 == xmlFieldParserFn[pTagDefn->eXMLTagDataType](pTagDefn->pXMLDataField, pTagDefn->uXMLDataFieldLength, pFieldData, uFieldLength))
            {
                XML_INFO_MSG("Error: Failed to parse field for [%s]", XML_Substring(pParserCtx->pCurrentTag, pParserCtx->uCurrentTagLength));
            }
        }

    }

    return eLocalState;
}





/* XML_ParseFieldString()
 *  - Parse a field in to a string
 */
static uint32 XML_ParseFieldString(void *pOutput, uint32 uMaxOutputLen, xmlchar *pInput, uint32 uInputLen)
{
    xmlchar  *pOutputString = (xmlchar*)pOutput;
    uint32   uParseLength = (uMaxOutputLen<uInputLen)?uMaxOutputLen:uInputLen;
    int32 i = 0;
    int32 c = 0;

    // Trim leading spaces
    while ((i<(int32)uParseLength) && (XML_IS_WHITESPACE(pInput[i])))
    {
        i++;
    }
    
    // Copy string to output

    while ((i<(int32)uParseLength))
    {
        pOutputString[c++] = pInput[i++];
    }

    // Trim trailing spaces
    if (c)
    {
        c--;
        while ((c>0) && (XML_IS_WHITESPACE(pOutputString[c])))
        {
            c--;
        }
        if (c>0)
            pOutputString[c+1] = XML_TAG_NULL;
        else
            pOutputString[c] = XML_TAG_NULL;
    }

    XML_INFO_MSG("Info: String tag [%s]", (xmlchar*)pOutput);

    return c;
}




/* XML_ParseFieldInteger()
 *  - Parse a field in to an integer handles, decimal and hex values
 */
static uint32 XML_ParseFieldInteger(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen)
{
    int32   uDataRemaining = (int32)uInputLen;
    xmlchar *pData = pInput;

    // Trim leading spaces
    while ((uDataRemaining>0) && (XML_IS_WHITESPACE(pData[0])))
    {
        uDataRemaining--;
        pData++;
    }

    // Check to see if this is a hex string
    if (uDataRemaining)
    {
        if ((uDataRemaining>2) && ('0'==pData[0]) && ('X'==XML_TOUPPER(pData[1])))
        {
           uDataRemaining -= ParseHexValue(pData, uDataRemaining, pOutput, uMaxSize);
        }
        else // Standard decimal digit
        {
            uDataRemaining -= ParseDecimalValue(pData, uDataRemaining, pOutput, uMaxSize);
        }
    }

    XML_INFO_MSG("Info: Integer tag [%i (0x%x)]", *(uint32*)pOutput, *(uint32*)pOutput);

    return (uInputLen - uDataRemaining);
}





/* XML_ParseFieldBoolean()
 *  - Parse a field in to a boolean
 */
static uint32 XML_ParseFieldBoolean(void *pOutput, uint32 uMaxOutputLen, xmlchar *pInput, uint32 uInputLen)
{
    bool32   bResult = FALSE;
    uint32 i = 0;

    UNREFERENCED_PARAMETER(uMaxOutputLen);

    // Trim leading spaces
    while ((i<uInputLen) && (XML_IS_WHITESPACE(pInput[i])))
    {
        i++;
    }
    
    // Simply look at the next character, '1', 'TRUE', 'true' all mean true
    if (i<=uInputLen)
    {
        if ((pInput[i]=='1') || (XML_TOUPPER(pInput[i])=='T'))
            bResult = TRUE;
    }

    *(bool32*)pOutput = bResult;

    XML_INFO_MSG("Info: Bool tag [%s]", (bResult)?"TRUE":"FALSE");

    return uInputLen;
}






/* XML_ParseFieldBinary()
 *  - Parse a field in to an binary
 */
static uint32 XML_ParseFieldBinary(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen)
{
    int32   uDataRemaining = (int32)uInputLen;
    xmlchar *pData = pInput;
    xmlchar *pOutputData;
    uint16  uCount = 0;
    XML_BinaryPacketType  *pBinaryPacket = (XML_BinaryPacketType*)pOutput;
    uint32  uDataSize = sizeof(pBinaryPacket->uPacketLength);

    // Set output to start after teh binary packet header
    pOutputData = (xmlchar*)&pBinaryPacket->pData;

    // Check to see if this is a hex string
    while ((uDataRemaining>0) && (uDataSize < uMaxSize))
    {
        if (XML_IS_WHITESPACE(pData[0]))
        {
            if (XML_IS_CARRIAGERETURN(pData[0]))
            {
                // Carriage return indicate a packet is complete
                if (uCount)
                {
                    pBinaryPacket->uPacketLength = uCount;
                    // Move up our packet
                    pBinaryPacket = (XML_BinaryPacketType*)pOutputData;
                    // Start at the data after the header
                    pOutputData = (xmlchar*)&pBinaryPacket->pData;
                    // Increment the total size
                    uDataSize += sizeof(pBinaryPacket->uPacketLength) + uCount;
                    // Clear the count
                    uCount = 0;
                }
            }

            pData++;
            uDataRemaining--;
        }
        else if (uDataRemaining>=2)
        {
            uint32 uDataProcessed;

            // Parse single hex byte 
            uDataProcessed = ParseHexValue(pData, uDataRemaining, pOutputData, sizeof(uint8));

            if (uDataProcessed)
            {
                pOutputData++;
                pData += uDataProcessed;
                uDataRemaining -= uDataProcessed;
                uCount++;
            }
            else
            {
                //XML_ERROR_MSG("Error: Invalid hex digit encountered [%s]", XML_Substring(pData, 2) );
                XML_ERROR_MSG("Error: Invalid hex digit encountered");
                return 0;
            }
        }
        else
        {
            pData++;
            uDataRemaining--;
        }
    }
    // Close off the last packet.
    if (uCount)
    {
        pBinaryPacket->uPacketLength = uCount;
        // Move up our packet
        pBinaryPacket = (XML_BinaryPacketType*)pOutputData;
        // Start at the data after the header
        pOutputData = (xmlchar*)&pBinaryPacket->pData;
    }

    // Pad the final output with zero, a zero sized packet indicates end of binary data.
    if ((uDataSize+sizeof(pBinaryPacket->uPacketLength))<=uMaxSize)
    {
        pBinaryPacket = (XML_BinaryPacketType*)pOutputData;
        pBinaryPacket->uPacketLength = 0;
    }
    else
    {
        XML_ERROR_MSG("Error: Output data has exceeded output buffer size");
    }

    XML_INFO_MSG("Info: Binary tag packet total size %llu bytes", (uint64)(uDataSize + sizeof(pBinaryPacket->uPacketLength)));

    return (uInputLen - uDataRemaining);
}



/* XML_ParseFieldGUID()
 *  - Parse a field in to an GUID
 */
static uint32 XML_ParseFieldGUID(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen)
{
    int32       uDataRemaining = (int32)uInputLen;
    xmlchar    *pData = pInput;
    XML_GUID   *pGUID = (XML_GUID*)pOutput;
    
    UNREFERENCED_PARAMETER(uMaxSize);

    // Search for starting brace
    while ((uDataRemaining>0) && (pData[0] != '{'))
    {
        pData++;
        uDataRemaining--;
    }

    if (uDataRemaining)
    {
        uint32 uCount;
        uint32 uElementsProcessed = 0;
        uint32 uDataProcessed = 0;

        // Skip the '{' token
        pData++;
        uDataRemaining--;

        // Parse the first DWORD
        if ((uDataProcessed = ParseHexValue(pData, uDataRemaining, &pGUID->Data1, sizeof(uint32)))>0)
        {
            uElementsProcessed++;
            uDataRemaining -= uDataProcessed;
            pData += uDataProcessed;
        }
        

        // Parse the next two words
        for (uCount=0;uCount<2;uCount++)
        {
            void *pDataPtr[2];

            pDataPtr[0] = (void*)&pGUID->Data2;
            pDataPtr[1] = (void*)&pGUID->Data3;

            // Skip to the next number token
            while ((uDataRemaining>0) && (!XML_IS_NUMERIC(pData[0])))
            {
                 pData++;
                 uDataRemaining--;
            }

            // Parse the first DWORD
            if ((uDataProcessed = ParseHexValue(pData, uDataRemaining, pDataPtr[uCount], sizeof(uint16)))>0)
            {
                uElementsProcessed++;
                uDataRemaining -= uDataProcessed;
                pData += uDataProcessed;
            }
        }


        // Parse the 8 bytes
        for (uCount=0;uCount<8;uCount++)
        {
            // Skip to the next number token
            while ((uDataRemaining>0) && (!XML_IS_NUMERIC(pData[0])))
            {
                 pData++;
                 uDataRemaining--;
            }

            // Parse the first DWORD
            if ((uDataProcessed = ParseHexValue(pData, uDataRemaining, &pGUID->Data4[uCount], sizeof(uint8)))>0)
            {
                uElementsProcessed++;
                uDataRemaining -= uDataProcessed;
                pData += uDataProcessed;
            }
        }

        // Check to ensure we processed all of the GUID elements
        if (XML_PARSER_MAX_GUID_FIELDS != uElementsProcessed)    
        {
            XML_ERROR_MSG("Error: Invalid GUID");
            return 0;
        }
    }

    XML_INFO_MSG("Info: GUID tag [%x-%x-%x-%x-%x-%x-%x-%x-%x-%x-%x]", pGUID->Data1, pGUID->Data2, pGUID->Data3, pGUID->Data4[0], pGUID->Data4[1], pGUID->Data4[2],
                                                                      pGUID->Data4[3], pGUID->Data4[4], pGUID->Data4[5], pGUID->Data4[6], pGUID->Data4[7]);

    return (uInputLen - uDataRemaining);
}




/* XML_ParseFieldIntList()
*  - Parse an integer list (Function deprecated by XML_ParseFieldHexList)
*/
static uint32 XML_ParseFieldIntList(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen)
{
    int32   uDataRemaining = (int32)uInputLen;
    xmlchar *pData = pInput;
    uint16  uCount = 0;
    uint32  *pIntegerOutput = (uint32*)pOutput;
    uint32  uDataSize = 0;

    // Check to see if this is a hex string
    while ((uDataRemaining>0) && (uDataSize < uMaxSize))
    {
        if (XML_IS_WHITESPACE(pData[0]))
        {
            if (XML_IS_CARRIAGERETURN(pData[0]))
            {
                // Carriage return indicate a packet is complete
                if (uCount)
                {
                    // Jump to the next dword
                    pIntegerOutput++;

                    // Increment the total size
                    uDataSize += sizeof(uint32);
                }
            }

            pData++;
            uDataRemaining--;
        }
        else if (uDataRemaining > 0)
        {
            uint32 uDataProcessed;

            // Parse single hex byte 
            uDataProcessed = ParseHexValue(pData, uDataRemaining, pIntegerOutput, sizeof(uint32));

            if (uDataProcessed)
            {
                // Jump to the next dword
                pIntegerOutput++;

                // Increment the total size
                uDataSize += sizeof(uint32);

                // Increment data pointer
                pData += uDataProcessed;

                // Subtract the amount of data processed
                uDataRemaining -= uDataProcessed;
            }
            else
            {
                // Increment data pointer, skip over the invalid character
                pData += uDataRemaining;

                // Subtract the amount of data processed
                uDataRemaining -= uDataRemaining;
            }
        }
        else
        {
            pData++;
            uDataRemaining--;
        }
    }

    XML_INFO_MSG("Info: Total DWORD read %llu", (uint64)(uDataSize / sizeof(uint32)));

    return (uInputLen - uDataRemaining);
}


/* XML_ParseFieldHexList()
 *  - Parse an hex list (can be both decimal and hex digits)
 */
static uint32 XML_ParseFieldHexList(void *pOutput, uint32 uMaxSize, xmlchar *pInput, uint32 uInputLen)
{
    int32   uDataRemaining = (int32)uInputLen;
    xmlchar *pData = pInput;
    uint16  uCount = 0;
    uint32  *pIntegerOutput = (uint32*)pOutput;
    uint32  uDataSize = 0;
    
    // Check to see if this is a hex string
    while ((uDataRemaining>0) && (uDataSize < uMaxSize))
    {
        if (XML_IS_WHITESPACE(pData[0]))
        {
            if (XML_IS_CARRIAGERETURN(pData[0]))
            {
                // Carriage return indicate a packet is complete
                if (uCount)
                {
                    // Jump to the next dword
                    pIntegerOutput++;

                    // Increment the total size
                    uDataSize += sizeof(uint32);
                }
            }

            pData++;
            uDataRemaining--;
        }
        else if (uDataRemaining>0)
        {
            uint32 uDataProcessed;

            // Parse single hex byte 
            if ((uDataRemaining>2) && ('0' == pData[0]) && ('X' == XML_TOUPPER(pData[1])))
            {
                uDataProcessed = ParseHexValue(pData, uDataRemaining, pIntegerOutput, sizeof(uint32));
            }
            else // Standard decimal digit
            {
                uDataProcessed = ParseDecimalValue(pData, uDataRemaining, pIntegerOutput, sizeof(uint32));
            }

            if (uDataProcessed)
            {
                // Jump to the next dword
                pIntegerOutput++;

                // Increment the total size
                uDataSize += sizeof(uint32);

                // Increment data pointer
                pData += uDataProcessed;

                // Subtract the amount of data processed
                uDataRemaining -= uDataProcessed;
            }
            else
            {
               // Increment data pointer, skip over the invalid character
                pData += uDataRemaining;

                // Subtract the amount of data processed
                uDataRemaining -= uDataRemaining;
            }
        }
        else
        {
            pData++;
            uDataRemaining--;
        }
    }

    XML_INFO_MSG("Info: Total DWORD read %llu", (uint64)(uDataSize / sizeof(uint32)));

    return (uInputLen - uDataRemaining);
}



/* ParseDecimalValue()
 *  - Parse a decimal string 
 */
static uint32 ParseDecimalValue(xmlchar *pInput, uint32 uInputLen, void *pOutput, uint32 uOutputSize)
{
    int32    iSign = 1;
    int32    iValue = 0;
    int32    uDigitsProcessed = 0;
    int32    uDataRemaining = (int32)uInputLen;
    xmlchar *pChar = pInput;

    // Skip whitespaces
    while (XML_IS_WHITESPACE(*pChar) && (uDataRemaining))
    {
        pChar++;
        uDigitsProcessed++;
    }

    // Start processing characters
    if (uDataRemaining)
    {
        // Check for sign
        if (pChar[0] == '-')
        {
            iSign = -1;
            pChar++;
            uDataRemaining--;
        }

        // Scan to the end of the string
        while ((uDataRemaining) && (uDigitsProcessed<XML_PARSER_MAX_DECIMAL_DIGITS) && (XML_IS_NUMERIC(*pChar)))
        {
            pChar++;
            uDigitsProcessed++;
            uDataRemaining--;
        }

        // If we have valid digits
        if (uDigitsProcessed)
        {
            uint32 uIndex;
            int32  iMultiplier = 1;

            // Go back to last character
            pChar--;

            // Start processing from reverse
            do 
            {
                uint32 digit = (uint32) (*pChar - '0');

                iValue += (digit*iMultiplier);
                pChar--;
                iMultiplier *= 10;
                uDigitsProcessed--;
            }
            while (uDigitsProcessed);

            // Include the sign and store the result
            iValue *= iSign;

            // Store the result (copy a byte at a time to ensure we only copy the maximum amount
            for (uIndex=0;uIndex<uOutputSize;uIndex++)
            {
                ((uint8*)pOutput)[uIndex] = ((uint8*)&iValue)[uIndex];
            }
        }
        else
        {
            // no data processed
            return 0;
        }
    }

    return uInputLen - uDataRemaining;
}



/* ParseHexValue()
 *  - Parse a hex string
 */
static uint32 ParseHexValue(xmlchar *pInput, uint32 uInputLen, void *pOutput, uint32 uOutputSize)
{
    uint32   uValue = 0;
    int32    uDigitsProcessed = 0;
    int32    uDataRemaining = (int32)uInputLen;
    xmlchar *pChar = pInput;

    // Skip whitespaces
    while (XML_IS_WHITESPACE(*pChar) && (uDataRemaining))
    {
        pChar++;
        uDigitsProcessed++;
    }

    // Parse leading '0x'
    if ((uDataRemaining>2) && ('0'==pChar[0]) && ('X'==XML_TOUPPER(pChar[1])))
    {
        uDataRemaining -= 2;
        pChar += 2;
    }

    // Start processing characters
    if (uDataRemaining)
    {
        // Scan to the end of the string
        while ((uDataRemaining) && (uDigitsProcessed<XML_PARSER_MAX_HEX_DIGITS) && (XML_IS_HEXDIGIT(*pChar)))
        {
            pChar++;
            uDigitsProcessed++;
            uDataRemaining--;
        }

        // If we have valid digits
        if (uDigitsProcessed)
        {
            uint32 uIndex;
            uint32 uShift = 0;

            // Go back to last character
            pChar--;

            // Start processing from reverse
            do 
            {
                uint8 digit = (uint32)(pChar[0]<='9')?(pChar[0] - '0'):(XML_TOUPPER(pChar[0]) - 'A' + 10);

                uValue |= (digit<<uShift);

                pChar--;
                uShift+=4;
                uDigitsProcessed--;
            }
            while (uDigitsProcessed);

            // Store the result (copy a byte at a time to ensure we only copy the maximum amount
            for (uIndex=0;uIndex<uOutputSize;uIndex++)
            {
                ((uint8*)pOutput)[uIndex] = ((uint8*)&uValue)[uIndex];
            }
        }
        else
        {
            // no data processed
            return 0;
        }
    }

    return uInputLen - uDataRemaining;
}


#ifdef __cplusplus
  }
#endif
