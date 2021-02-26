#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_
/*=============================================================================
 
  File: xmlparser.h
  
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/xmlparser.h#1 $$
  
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "xmltags.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Debug message level */
#define XML_MSG_LEVEL_ERROR               1
#define XML_MSG_LEVEL_INFO                2

/* Enable debugging messages */
#define XML_DEBUG_MSG                     0

/* Parser return codes
 */
#define XML_PARSER_STATUS_OK            0x0 
#define XML_PARSER_STATUS_BADPARAM      0x1

#define XML_CASE_INSENSITIVE              1

/*  Defines
 */
#define XML_HEADER_TAG         "?xml"
#define XML_HEADER_TAG_END     "?>"

#define XML_COMMENT_TAG        "!--"
#define XML_COMMENT_END        "-->"

#define XML_TAG_ENTRY_START    "<"
#define XML_TAG_EXIT_START     "</"
#define XML_TAG_ENTRY_END      ">"
#define XML_TAG_SHORTENTRY_END "/>"

#define XML_TAG_NULL            '\0'
#define XML_TAG_WHITESPACE      ' '
#define XML_TAG_CARRIAGERETURN  '\n'
#define XML_TAG_LINEFEED        '\r'
#define XML_TAG_TAB             '\t'

#define XML_TAG_CSV             ','

#define XML_GROUP_TAG           "Group"

#if (XML_MSG_LEVEL_ERROR & XML_MSG_LEVEL)
#define XML_ERROR_MSG(format, ...) DISP_LOG_FN(format, ##__VA_ARGS__);
#else
    #define XML_ERROR_MSG(format, ...)
#endif

#if (XML_MSG_LEVEL_INFO & XML_MSG_LEVEL)
    #define XML_INFO_MSG(format, ...) DISP_LOG_FN(format, ##__VA_ARGS__);
#else
    #define XML_INFO_MSG(format, ...)
#endif

/*  Helper functions
 */
#define XML_CLEAR_STRING(_str_) (_str_[0] ='\0')

#define XML_TOUPPER(_c_) ((_c_)&(xmlchar)(~0x20))

#if (XML_CASE_INSENSITIVE==1)
    #define XML_COMPARE_CHAR(_a_,_b_) (XML_TOUPPER(_a_)==XML_TOUPPER(_b_))
#else
    #define XML_COMPARE_CHAR(_a_,_b_) ((_a_)==(_b_))
#endif

#define XML_IS_CARRIAGERETURN(_c_) (XML_TAG_CARRIAGERETURN==(_c_) || XML_TAG_LINEFEED==(_c_))

#define XML_IS_WHITESPACE(_c_) ((XML_TAG_NULL==(_c_))||(XML_TAG_WHITESPACE==(_c_))||(XML_TAG_CARRIAGERETURN==(_c_))||(XML_TAG_TAB==(_c_))||(XML_TAG_LINEFEED==(_c_)))

#define XML_IS_PUNCATION(_c_) (((_c_)>='!' && (_c_)<='/'))

#define XML_IS_ALPHABET(_c_)  ((_c_)>='A' && (_c_)<='z')

#define XML_IS_NUMERIC(_c_) (((_c_)>='0' && (_c_)<='9'))

#define XML_IS_HEXDIGIT(_c_) (((_c_)>='0' && (_c_)<='9') || (XML_TOUPPER(_c_)>='A' && XML_TOUPPER(_c_)<='F'))

#define XML_IS_ALPHANUMERIC(_c_) (XML_IS_PUNCATION(_c_) || XML_IS_ALPHABET(_c_) || XML_IS_NUMERIC(_c_))

 
/* Parser enumerations
 */
typedef enum {
    XML_STATE_INIT = 0,
    XML_STATE_TAG_SEARCH,
    XML_STATE_TAG_FOUND,
    XML_STATE_DONE,
    XML_STATE_ERROR,
    XML_STATE_MAX
} XML_ParserStateType;


/* Parser Structures
 *  
 */

/* Binary packetized format 
 */
#pragma pack(push, 1) // exact fit - no padding
typedef struct {
    uint32  uPacketLength;
    uint8*  pData;
} XML_BinaryPacketType;
#pragma pack(pop)     //back to whatever the previous packing mode was


/* GUID format
 */
typedef struct {
  uint32 Data1;
  uint16 Data2;
  uint16 Data3;
  uint8  Data4[8];
} XML_GUID;




typedef struct 
{
    /* XML parser state machine state */
    XML_ParserStateType  eParserState;

    /* XML Header information */
    xmlchar         sXmlVersion[XML_MAX_STRING_LENGTH];
    xmlchar         sXmlEncoding[XML_MAX_STRING_LENGTH];

    /* Pointers to the current xml data */
    xmlchar         *pDataStartPtr;
    xmlchar         *pCurrentDataPtr;
    int32           iDataAvailable;

    /* Pointers to the XML tag data */    
    XML_TagDefType *pXmlTagList; 
    uint32          uXmlTagListLength;

    /* State information */
    xmlchar         *pCurrentTag;
    uint32          uCurrentTagLength;     

} XML_ParserContextType;


/* Internal parser state machine functions
 */
typedef uint32 (* XML_ParserFnType)(XML_ParserContextType *pParserCtx);


typedef uint32 (* XML_FieldParserFnType)(void *pOutput, uint32 uMaxOutputLen, xmlchar *pInput, uint32 uInputLen);


/* Main XML parser function
 */
uint32 XML_Parser(void              *pXmlData,
                  uint32             uDataLength,
                  XML_TagDefType    *pXmlTagList,
                  uint32             uMaxTags);


/* XML parser function which only parses within a group context, ex <group id="GroupName">  </group>
 */
uint32 XML_ParserGroup(xmlchar        *pGroupName,
                            void           *pXmlData,
                            uint32          uDataLength,
                            XML_TagDefType *pXmlTagList,
                            uint32          uMaxTags);
                            

/* XML Utility to parse CVS fields from a string
 */
uint32 XML_ParseCSV(void *pData,
                    uint32 uDataLength,
                    uint32 uFieldNum,
                    XML_TagDataType eDataType,
                    void *pOuputData,
                    uint32 uOutputSize);

/* Update parsed XML output data within a XML tag list for given XML Tag 
 */
uint32 XML_UpdateTag( XML_TagDefType    *pXmlTagList,
                      uint32             uMaxTags,
                      uint8             *pXMLTag,
                      uint32             uXMLTagLength,
                      void              *pXMLDataField,
                      uint16             uXMLDataFieldLength);
#ifdef __cplusplus
 }
#endif

#endif // _XML_PARSER_H_
